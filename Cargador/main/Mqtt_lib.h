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



esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event);
void mqtt_app_start(char *broker);
void mqtt_config();
void mqtt_init(char *broker, char *input_topic, char *output_topic);
void SetValues(char *input_topic, char *output_topic);
void sendMessage(char *data, char* topic);
