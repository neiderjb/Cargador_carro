#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"


esp_err_t wifi_event_handler(void *ctx, system_event_t *event);
void wifi_begin(char *ssid, char *password);
bool Isconnected();
void ResetCount();