#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "wifi_lib.h"

static const char *TAG = "WIFI";
static EventGroupHandle_t wifi_event_group;
const static int CONNECTED_BIT = BIT0;
bool connected_value = true;
int countConnection = 0;

esp_err_t wifi_event_handler(void *ctx, system_event_t *event)
{
	switch (event->event_id)
	{
	case SYSTEM_EVENT_STA_START:
		esp_wifi_connect();
		break;
	case SYSTEM_EVENT_STA_GOT_IP:
		xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);

		break;
	case SYSTEM_EVENT_STA_DISCONNECTED:
		esp_wifi_connect();
		countConnection++;
		if (countConnection > 1)
		{
			connected_value = false;
		}
		else
		{
			connected_value = true;
			xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
		}
		break;
	default:
		break;
	}
	return ESP_OK;
}

void wifi_begin(char *ssid, char *password)
{

	wifi_event_group = xEventGroupCreate();
	tcpip_adapter_init();
	//ESP_ERROR_CHECK(esp_event_loop_create_default());
	ESP_ERROR_CHECK(esp_event_loop_init(wifi_event_handler, NULL));
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	//ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

	static wifi_config_t wifi_config = {};
	strcpy((char *)wifi_config.sta.ssid, (const char *)ssid);
	strcpy((char *)wifi_config.sta.password, (const char *)password);

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));

	ESP_LOGI(TAG, "start the WIFI SSID:[%s]", ssid);
	ESP_ERROR_CHECK(esp_wifi_start());
	ESP_LOGI(TAG, "Waiting for wifi");

	xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true,
						15000 / portTICK_PERIOD_MS);
	if (!connected_value)
	{
		ESP_LOGI(TAG, "ENTRADA !");
		esp_wifi_stop();
		esp_wifi_deinit();
	}
	ESP_LOGI("Wifi Control", "Begin OK");
}

bool Isconnected()
{
	if (connected_value)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ResetCount()
{
	countConnection = 0;
}
