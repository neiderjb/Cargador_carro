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
#include "mqtt_client.h"
#include "Parameters.h"
#include "cJSON.h"
#include "PCF85063TP.h"
#include "wifi_lib.h"
#include "FunctionsCC.h"
#include "functions.h"
#include "EPLD.h"

static const char *TAG = "MQTT";
char *input;
char *output;
esp_mqtt_client_handle_t client = NULL;

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
		/*
		msg_id = esp_mqtt_client_subscribe(client, "airis/cc/start", 1);
		ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

		msg_id = esp_mqtt_client_subscribe(client, "airis/cc0001/commands", 1);
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
		//ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
		break;
	case MQTT_EVENT_UNSUBSCRIBED:
		ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
		break;
	case MQTT_EVENT_PUBLISHED:
		//ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
		break;
	case MQTT_EVENT_DATA:
		ESP_LOGI(TAG, "MQTT_EVENT_DATA");
		char *s = event->data;
		GetCommandsMqtt(s);
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
	esp_mqtt_client_publish(client, topic, data, 0, 1, 0);
	ESP_LOGI(TAG, "sent publish successful");
	ESP_LOGW(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
}

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
