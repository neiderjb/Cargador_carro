/****************************************************************************/
//  Function: file for I2C
//  Hardware: AIRIS - RTC
//  ESP32 MCU: ESPRESSIF
//  Author:   Heberth Martinez - Diego Ochoa
//  Date:    July 11,2019
//  Version: v1.0
//  by Agrum.SAS
/****************************************************************************/


#include <driver/i2c.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>

void i2c_list_devices();

void i2c_configuration(i2c_port_t i2c_num, uint8_t SDA_PIN, uint8_t SCL_PIN);



esp_err_t i2c_uart_write_ZDU0210RJX(i2c_port_t i2c_num, uint8_t DEVICE_ADDR, uint8_t *data, int count, uint8_t reg);
esp_err_t i2c_uart_write_ZDU0210RJX_8(i2c_port_t i2c_num, uint8_t DEVICE_ADDR, uint8_t data, uint8_t reg);
esp_err_t i2c_uart_write_read_ZDU0210RJX(i2c_port_t i2c_num, uint8_t DEVICE_ADDR, uint8_t *dataWrite, int countWrite, uint8_t reg, uint8_t *dataRead, int countRead);

esp_err_t i2c_uart_read_ZDU0210RJX(i2c_port_t i2c_num, uint8_t i2c_addr, uint8_t *data_rd , int count, uint8_t reg);
esp_err_t i2c_uart_read_16_ZDU0210RJX(i2c_port_t i2c_num, uint8_t DEVICE_ADDR, uint8_t *data_rd , uint8_t reg);

esp_err_t i2c_uart_enable_ZDU0210RJX(i2c_port_t i2c_num, uint8_t DEVICE_ADDR, uint8_t data, uint8_t reg);



esp_err_t reset_fifo_bus_uart_1(i2c_port_t i2c_num, uint8_t DEVICE_ADDR);
esp_err_t i2c_uart_1_read_baudrate(i2c_port_t i2c_num, uint8_t DEVICE_ADDR, uint8_t *data);
esp_err_t i2c_uart_1_read_FIFO_Level(i2c_port_t i2c_num, uint8_t i2c_addr, uint8_t *data_rd);



esp_err_t i2c_uart_read(i2c_port_t i2c_num, uint8_t DEVICE_ADDR, uint8_t REG_ADDR, uint8_t *data);
esp_err_t i2c_gpio_configuration(i2c_port_t i2c_num, uint8_t DEVICE_ADDR, uint8_t gpio, uint8_t mode);
esp_err_t i2c_gpio_write(i2c_port_t i2c_num, uint8_t DEVICE_ADDR, uint8_t number_gpio, uint8_t status);
uint8_t i2c_gpio_read(i2c_port_t i2c_num, uint8_t DEVICE_ADDR);
esp_err_t i2c_uart_read_FIFO(i2c_port_t i2c_num, uint8_t DEVICE_ADDR, uint8_t *data, size_t cnt, uint8_t uart);


esp_err_t i2c_spi_write(i2c_port_t i2c_num, uint8_t DEVICE_ADDR, uint8_t cmdByte, uint8_t *data, int count);
esp_err_t i2c_spi_read(i2c_port_t i2c_num, uint8_t DEVICE_ADDR,  uint8_t *data, int count);

esp_err_t i2c_write(i2c_port_t i2c_num, uint8_t DEVICE_ADDR, uint8_t *data, int count);
esp_err_t i2c_read(i2c_port_t i2c_num, uint8_t DEVICE_ADDR, uint8_t DEVICE_REG, uint8_t *data,  int count);
