/**************************/
//  Function: file for I2C
//  Hardware: AIRIS - RTC
//  ESP32 MCU: ESPRESSIF
//  Author:   Heberth Martinez - Diego Ochoa
//  Date:    July 11,2019
//  Version: v1.0
//  by Agrum.SAS
/**************************/

#include <driver/i2c.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>

#include "esp_log.h"

static const char *TAG = "i2c";

// I2C common protocol defines
#define WRITE_BIT I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ   /*!< I2C master read */
#define ACK_CHECK_EN 0x1		   /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0		   /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0				   /*!< I2C ack value */
#define NACK_VAL 0x1			   /*!< I2C nack value */
#define SPEED 30000				   //100000	50000

void i2c_list_devices()
{
	esp_err_t espRc;
	printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");
	printf("00:         ");
	for (int i = 3; i < 0x78; i++)
	{
		i2c_cmd_handle_t cmd = i2c_cmd_link_create();
		i2c_master_start(cmd);
		i2c_master_write_byte(cmd, (i << 1) | I2C_MASTER_WRITE, 1);
		i2c_master_stop(cmd);

		espRc = i2c_master_cmd_begin(I2C_NUM_0, cmd, 10 / portTICK_PERIOD_MS);
		if (i % 16 == 0)
		{
			printf("\n%.2x:", i);
		}
		if (espRc == 0)
		{
			printf(" %.2x", i);
		}
		else
		{
			printf(" --");
		}

		i2c_cmd_link_delete(cmd);
	}
	printf("\n");
}

void i2c_configuration(i2c_port_t i2c_num, uint8_t SDA_PIN, uint8_t SCL_PIN)
{
	i2c_config_t conf;
	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = SDA_PIN;
	conf.scl_io_num = SCL_PIN;
	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf.master.clk_speed = SPEED;
	i2c_param_config(i2c_num, &conf);
	i2c_driver_install(i2c_num, I2C_MODE_MASTER, 0, 0, 0);

	ESP_LOGI(TAG, "i2c_configuration OK");
}

//I2C device ZDU0210RJX

//Write 1 Byte
esp_err_t i2c_uart_write_ZDU0210RJX_8(i2c_port_t i2c_num, uint8_t DEVICE_ADDR, uint8_t data, uint8_t reg)
{
	int ret;
	//printf("WRITE ZDU0210RJX I2c Data 0: %x , count : %x \n", data[0], count);
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, DEVICE_ADDR << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, data, ACK_CHECK_EN);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return ret;
}

//Write multiple Bytes
esp_err_t i2c_uart_write_ZDU0210RJX(i2c_port_t i2c_num, uint8_t DEVICE_ADDR, uint8_t *data, int count, uint8_t reg)
{
	int ret;
	//printf("WRITE ZDU0210RJX I2c Data 0: %x , count : %x \n", data[0], count);
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, DEVICE_ADDR << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);

	for (int i = 0; i < count; i++)
	{
		i2c_master_write_byte(cmd, data[i], ACK_CHECK_EN);
	}
	//i2c_master_write(cmd, data, count, ACK_CHECK_EN);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return ret;
}
	
esp_err_t i2c_uart_read_ZDU0210RJX(i2c_port_t i2c_num, uint8_t DEVICE_ADDR, uint8_t *data_rd, int  count, uint8_t reg)
{
	int ret;
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (DEVICE_ADDR << 1) | I2C_MASTER_WRITE, 1 /* expect ack */);
	i2c_master_write_byte(cmd, (reg), 1 /* expect ack */);
	i2c_master_start(cmd);

	i2c_master_write_byte(cmd, (DEVICE_ADDR << 1) | I2C_MASTER_READ, 1 /* expect ack */);

	for(int i = 0; i<count-1; i++){
		i2c_master_read_byte(cmd, data_rd + i, ACK_VAL);
		//printf("READ Data UART 1: %s \n", data_rd[i]);
	}
	i2c_master_read_byte(cmd, data_rd + (count-1), NACK_VAL);

	// if (count > 1)
	// {
	// 	i2c_master_read(cmd, data_rd, count - 1, ACK_VAL);
	// }
	// i2c_master_read_byte(cmd, data_rd + count - 1, NACK_VAL);

	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);
	return ret;
}

//Write multiple Bytes
esp_err_t i2c_uart_write_read_ZDU0210RJX(i2c_port_t i2c_num, uint8_t DEVICE_ADDR, uint8_t *dataWrite, int countWrite, uint8_t reg, uint8_t *dataRead, int countRead)
{
	int ret;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, DEVICE_ADDR << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);

	for (int i = 0; i < countWrite; i++)
	{
		i2c_master_write_byte(cmd, dataWrite[i], ACK_CHECK_EN);
		//printf("WRITE GPRS I2c Data: %x , count : %x \n", dataWrite[i], i);
	}
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	vTaskDelay(2000);
	i2c_uart_read_ZDU0210RJX(i2c_num,DEVICE_ADDR,dataRead,countRead,(reg+1));

	return ret;
}



esp_err_t i2c_uart_read_16_ZDU0210RJX(i2c_port_t i2c_num, uint8_t DEVICE_ADDR, uint8_t *data_rd, uint8_t reg)
{
	int ret;
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (DEVICE_ADDR << 1) | I2C_MASTER_WRITE, 1 /* expect ack */);
	i2c_master_write_byte(cmd, (reg), 1 /* expect ack */);
	i2c_master_start(cmd);

	i2c_master_write_byte(cmd, (DEVICE_ADDR << 1) | I2C_MASTER_READ, 1 /* expect ack */);

	i2c_master_read_byte(cmd, data_rd, ACK_VAL);
	i2c_master_read_byte(cmd, data_rd + 1, NACK_VAL);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);
	return ret;
}

esp_err_t i2c_uart_enable_ZDU0210RJX(i2c_port_t i2c_num, uint8_t DEVICE_ADDR, uint8_t data, uint8_t reg)
{
	int ret;
	//printf("WRITE ENABLE ZDU0210RJX I2c Data 0: %x \n", data);
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, DEVICE_ADDR << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, data, ACK_CHECK_EN);
	;
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return ret;
}

/*
To configure the GPIO port in the gpio place '1' 
to the gpio that wants to change the current configuration 
and in the mode place '0' to set it as input and '1' 
to define it as output
*/

esp_err_t i2c_gpio_configuration(i2c_port_t i2c_num, uint8_t DEVICE_ADDR, uint8_t gpio, uint8_t mode)
{
	int ret;
	int count = 2;
	uint8_t data[] = {gpio, mode};
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, DEVICE_ADDR << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, 0x08, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, 0x01, ACK_CHECK_EN);
	i2c_master_write(cmd, data, count, ACK_CHECK_EN);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return ret;
}

/*
GPIO - STATUS 
1 = HIGH
0 = LOW
*/

esp_err_t i2c_gpio_write(i2c_port_t i2c_num, uint8_t DEVICE_ADDR, uint8_t number_gpio, uint8_t status)
{
	int ret;
	int count = 2;
	uint8_t data[] = {number_gpio, status};
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, DEVICE_ADDR << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, 0x06, ACK_CHECK_EN);
	i2c_master_write(cmd, data, count, ACK_CHECK_EN);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return ret;
}

uint8_t i2c_gpio_read(i2c_port_t i2c_num, uint8_t DEVICE_ADDR)
{
	uint8_t data = 0;
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, DEVICE_ADDR << 1 | I2C_MASTER_WRITE, true);
	i2c_master_write_byte(cmd, 0x07, true);
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, DEVICE_ADDR << 1 | I2C_MASTER_READ, true);
	i2c_master_read_byte(cmd, &data, 0);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	vTaskDelay(1000 / portTICK_RATE_MS);
	return data;
	/*

	int ret;
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	//i2c_master_write_byte(cmd, DEVICE_ADDR << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);
	//i2c_master_write_byte(cmd, 0x07, ACK_CHECK_EN);
	//i2c_master_start(cmd);
	i2c_master_write_byte(cmd, DEVICE_ADDR << 1 | I2C_MASTER_READ, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, 0x07, ACK_CHECK_EN);
	i2c_master_read_byte(cmd, data, NACK_VAL);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return ret;


	esp_err_t ret;
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, DEVICE_ADDR << 1 | I2C_MASTER_WRITE, true);
	i2c_master_write_byte(cmd, 0x07, true);
	i2c_master_start(cmd);

	i2c_master_write_byte(cmd, DEVICE_ADDR << 1 | I2C_MASTER_READ, true);

	if (cnt > 1) {
		i2c_master_read(cmd, data, cnt-1, I2C_MASTER_ACK);
	}
	i2c_master_read_byte(cmd, data+cnt-1, 0);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return ret;
*/
}

//I2C-SPI device - SC18IS602B

esp_err_t i2c_spi_write(i2c_port_t i2c_num, uint8_t DEVICE_ADDR, uint8_t cmdByte, uint8_t *data, int count)
{
	//printf("WRITE SPI Data 0: %x , Data 1: %x \n", data[0], data[1]);
	int ret;
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, DEVICE_ADDR << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);

	i2c_master_write_byte(cmd, cmdByte, ACK_CHECK_EN);

	for (int i = 0; i < count; i++)
	{
		i2c_master_write_byte(cmd, data[i], ACK_CHECK_EN);
	}

	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return ret;
}

esp_err_t i2c_spi_read(i2c_port_t i2c_num, uint8_t DEVICE_ADDR, uint8_t *data_rd, int count)
{
	esp_err_t ret;
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);

	i2c_master_write_byte(cmd, DEVICE_ADDR << 1 | I2C_MASTER_READ, true);

	if (count > 1)
	{
		i2c_master_read(cmd, data_rd, count - 1, I2C_MASTER_ACK);
	}
	i2c_master_read_byte(cmd, data_rd + count - 1, NACK_VAL);

	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);

	// for(int i=0; i< count; i++){
	// 		printf("READ I2C-SPI Device: %x , Data : %x \n", DEVICE_ADDR, data_rd[i]);
	// }

	return ret;
}

//I2C

esp_err_t i2c_write(i2c_port_t i2c_num, uint8_t DEVICE_ADDR, uint8_t *data, int count)
{
	//printf("WRITE I2c Data 0: %x , Data 1: %x \n", data[0], data[1]);
	int ret;
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, DEVICE_ADDR << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);

	for (int i = 0; i < count; i++)
	{
		i2c_master_write_byte(cmd, data[i], ACK_CHECK_EN);
	}

	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return ret;
}

esp_err_t i2c_read(i2c_port_t i2c_num, uint8_t DEVICE_ADDR, uint8_t DEVICE_REG, uint8_t *data, int count)
{

	esp_err_t ret;
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (DEVICE_ADDR << 1) | I2C_MASTER_WRITE, true);
	i2c_master_write_byte(cmd, DEVICE_REG, true);
	i2c_master_start(cmd);

	i2c_master_write_byte(cmd, DEVICE_ADDR << 1 | I2C_MASTER_READ, true);

	if (count > 1)
	{
		i2c_master_read(cmd, data, count - 1, I2C_MASTER_ACK);
	}
	i2c_master_read_byte(cmd, data + count - 1, NACK_VAL);

	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);

	//printf("READ I2C Device: %x , Data 0: %x , Data 1: %x \n", DEVICE_ADDR, data[0], data[1]);

	return ret;
}