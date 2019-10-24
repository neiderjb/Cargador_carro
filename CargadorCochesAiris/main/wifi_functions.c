
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"

#include "wifi_functions.h"
#include "Parameters.h"

static const char *TAG = "WIFI_FUNCTIONS";

bool connected_value_wifi = true;
int count = 0;

// Event group for wifi connection
static EventGroupHandle_t wifi_event_group;
const int CONNECTED_BIT = BIT0;

// Wifi event handler
static esp_err_t wifi_event_handler(void *ctx, system_event_t *event)
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
		count++;
		if (count > 1)
		{
			connected_value_wifi = false;
		}
		else
		{
			connected_value_wifi = true;
			xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
		}
		break;
	default:
		break;
	}
	return ESP_OK;
}

bool Isconnected_wifi()
{
	if (connected_value_wifi)
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
	count = 0;
}

bool wifi_initialise(char *ssid, char *password)
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

	return wifi_conection(ssid);
}

bool wifi_conection(char *ssid)
{

	ESP_LOGI(TAG, "start the WIFI SSID:[%s]", ssid);
	ESP_ERROR_CHECK(esp_wifi_start());

	wifi_wait_connected();

	if (!connected_value_wifi)
	{
		ESP_LOGI(TAG, "No connection Exit WIFI !");
		esp_wifi_stop();
		esp_wifi_deinit();
		return false;
	}
	else
	{
		// print the local IP address
		tcpip_adapter_ip_info_t ip_info;
		ESP_ERROR_CHECK(tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ip_info));
		printf("IP Address:  %s\n", ip4addr_ntoa(&ip_info.ip));
		printf("Subnet mask: %s\n", ip4addr_ntoa(&ip_info.netmask));
		printf("Gateway:     %s\n", ip4addr_ntoa(&ip_info.gw));
	}

	return true;
}

void wifi_wait_connected()
{
	ESP_LOGI(TAG, "Wait Connecting WIFI\n");
	xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, 10000 / portTICK_PERIOD_MS);
}