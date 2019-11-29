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

#ifndef _WIFI_FUNCTIONS_H_
#define _WIFI_FUNCTIONS_H_

//#define WIFI_SSID			"UNE_HFC_D860"
//#define WIFI_PASS			"AAA0CBF7"
//#define WIFI_SSID			"CEMUSA"
//#define WIFI_PASS			"Ofiled@803"

bool wifi_initialise(char *ssid, char *password);
void wifi_wait_connected();
bool wifi_conection(char *ssid);
bool Isconnected_wifi();
#endif