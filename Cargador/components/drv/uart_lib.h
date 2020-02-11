#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include <math.h>
#include "cJSON.h"

static const int RX_BUF_SIZE = 1024;

void uart_begin(int baudrate, int rx_pin, int tx_pin);
int sendData(const char* data);
int sendcaracter(const char* data, const int sizedata);