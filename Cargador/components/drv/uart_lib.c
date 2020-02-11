#include "uart_lib.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include <math.h>
#include "cJSON.h"

void uart_begin(int baudrate, int rx_pin, int tx_pin) {
    const uart_config_t uart_config = {
        .baud_rate = baudrate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
}


/*
Funcion a emplear cuando se desea enviar ascii por medio de la UART
*/
int sendData(const char* data)
{
    const int len = strlen(data);
    const int txBytes = uart_write_bytes(UART_NUM_1, data, len);
    ESP_LOGI("TX: ", "Wrote %d bytes -- %s", txBytes,data);
    return txBytes;
}

/*
Funcion a emplear cuando se desea enviar raw bytes (cabeceras - solo aplica a uart no a i2c-uart) por medio de la UART
*/
int sendcaracter(const char* data, const int sizedata)
{
    const int txBytes = uart_write_bytes(UART_NUM_1, data, sizedata);
    ESP_LOGI("TX: ", "Wrote %d bytes", txBytes);
    return txBytes;
}