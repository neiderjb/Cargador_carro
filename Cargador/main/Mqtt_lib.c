#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

//#include "esp_wifi.h"
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
#include "mqtt_client.h"
#include "Parameters.h"
#include "cJSON.h"

static const char *TAG = "MQTT";
char *input;
char *output;
esp_mqtt_client_handle_t client = NULL;


//static EventGroupHandle_t wifi_event_group;
//const static int CONNECTED_BIT = BIT0;
//bool connected_value = true;
//int count = 0;


/*
char *substring(char *string, int position, int length)
{
	char *pointer;
	int c;

	pointer = malloc(length + 1);

	if (pointer == NULL)
	{
		printf("Unable to allocate memory.\n");
		exit(1);
	}

	for (c = 0; c < length; c++)
	{
		*(pointer + c) = *(string + position - 1);
		string++;
	}

	*(pointer + c) = '\0';

	return pointer;
}*/

esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
	client = event->client;
	int msg_id;
	// your_context_t *context = event->context;
	switch (event->event_id)
	{
	case MQTT_EVENT_CONNECTED:
		
		msg_id = esp_mqtt_client_subscribe(client, input, 0);
		ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

		/*msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
		ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

		msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
		ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);*/
		break;
	case MQTT_EVENT_DISCONNECTED:
		ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
		break;

	case MQTT_EVENT_SUBSCRIBED:
		ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
		msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0,
										 0);
		ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
		break;
	case MQTT_EVENT_UNSUBSCRIBED:
		ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
		break;
	case MQTT_EVENT_PUBLISHED:
		ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
		break;
	case MQTT_EVENT_DATA:
		ESP_LOGI(TAG, "MQTT_EVENT_DATA");
		int values[8];
		char *s = event->data;
		cJSON *root, *dataarray;
		root = cJSON_CreateObject();
		root = cJSON_Parse(s);
		dataarray = cJSON_GetObjectItem(root,"command");
		int constant = 192;
		if (!xQueueSend(Queue_control, &constant, 100 / portTICK_PERIOD_MS))
		{
			printf("Error Send Queue!!\n");
		}
		for (int i = 0; i<cJSON_GetArraySize(dataarray);i++)
		{	
			values[i] = (cJSON_GetArrayItem(dataarray,i)->valueint);
			printf("Send to Queue %d: %d\n", i, values[i]);
			if (!xQueueSend(Queue_control, &values[i], 100 / portTICK_PERIOD_MS))
			{
				printf("Error Send Queue!!\n");
			}
		}
		xSemaphoreGive(Semaphore_control);
		break;
	case MQTT_EVENT_ERROR:
		ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
		break;
	default:
		ESP_LOGI(TAG, "Other event id:%d", event->event_id);
		break;
	}
	return ESP_OK;
}

void sendMessage(char *data, char *topic)
{
	int msg_id = esp_mqtt_client_publish(client, topic, data, 0, 1, 0);
	ESP_LOGI(TAG, "data size, data=%d", sizeof(data));
	ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
}


// esp_err_t wifi_event_handler(void *ctx, system_event_t *event)
// {
// 	switch (event->event_id)
// 	{
// 	case SYSTEM_EVENT_STA_START:
// 		esp_wifi_connect();
// 		break;
// 	case SYSTEM_EVENT_STA_GOT_IP:
// 		xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);

// 		break;
// 	case SYSTEM_EVENT_STA_DISCONNECTED:
// 		esp_wifi_connect();
// 		count++;
// 		if (count > 1)
// 		{
// 			connected_value = false;
// 		}
// 		else
// 		{
// 			connected_value = true;
// 			xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
// 		}
// 		break;
// 	default:
// 		break;
// 	}
// 	return ESP_OK;
// }

// bool Isconnected()
// {
// 	if (connected_value)
// 	{
// 		return true;
// 	}
// 	else
// 	{
// 		return false;
// 	}
// }

// void ResetCount()
// {
// 	count = 0;
// }

// void wifi_init(char *ssid, char *password)
// {

// 	wifi_event_group = xEventGroupCreate();
// 	tcpip_adapter_init();
// 	//ESP_ERROR_CHECK(esp_event_loop_create_default());
// 	ESP_ERROR_CHECK(esp_event_loop_init(wifi_event_handler, NULL));
// 	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
// 	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

// 	//ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

// 	static wifi_config_t wifi_config = {};
// 	strcpy((char *)wifi_config.sta.ssid, (const char *)ssid);
// 	strcpy((char *)wifi_config.sta.password, (const char *)password);

// 	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
// 	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));

// 	ESP_LOGI(TAG, "start the WIFI SSID:[%s]", ssid);
// 	ESP_ERROR_CHECK(esp_wifi_start());
// 	ESP_LOGI(TAG, "Waiting for wifi");

// 	xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true,
// 						15000 / portTICK_PERIOD_MS);
// 	if (!connected_value)
// 	{
// 		ESP_LOGI(TAG, "ENTRADA !");
// 		esp_wifi_stop();
// 		esp_wifi_deinit();
// 	}
// }

void SetValues(char *input_topic, char *output_topic)
{
	input = input_topic;
	output = output_topic;
}

void mqtt_app_start(char *broker)
{
	esp_mqtt_client_config_t mqtt_cfg = {
		.uri = broker,
		.event_handle = mqtt_event_handler,
		// .user_context = (void *)your_context
	};

#if CONFIG_BROKER_URL_FROM_STDIN
	char line[128];

	if (strcmp(mqtt_cfg.uri, "FROM_STDIN") == 0)
	{
		int count = 0;
		printf("Please enter url of mqtt broker\n");
		while (count < 128)
		{
			int c = fgetc(stdin);
			if (c == '\n')
			{
				line[count] = '\0';
				break;
			}
			else if (c > 0 && c < 127)
			{
				line[count] = c;
				++count;
			}
			vTaskDelay(10 / portTICK_PERIOD_MS);
		}
		mqtt_cfg.uri = line;
		printf("Broker url: %s\n", line);
	}
	else
	{
		ESP_LOGE(TAG, "Configuration mismatch: wrong broker url");
		abort();
	}
#endif /* CONFIG_BROKER_URL_FROM_STDIN */

	esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
	esp_mqtt_client_start(client);
}

void mqtt_config()
{
	//Wifi Configuration
	ESP_LOGI(TAG, "[APP] Startup..");
	ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
	ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

	esp_log_level_set("*", ESP_LOG_INFO);
	esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
	esp_log_level_set("TRANSPORT_TCP", ESP_LOG_VERBOSE);
	esp_log_level_set("TRANSPORT_SSL", ESP_LOG_VERBOSE);
	esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
	esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);
}

void mqtt_init(char *broker, char *input_topic, char *output_topic)
{
	SetValues(input_topic, output_topic);
	mqtt_app_start(broker);
}
