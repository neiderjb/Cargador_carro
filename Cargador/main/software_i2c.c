/*

Copyright (c) 2018-2019 Mika Tuupola

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <driver/gpio.h>
#include <driver/i2c.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_err.h>
#include <esp_log.h>
#include "software_i2c.h"

static bool g_i2c_started;
static uint8_t g_i2c_sda;
static uint8_t g_i2c_scl;

#define LOW 0x00
#define HIGH 0x01

#define CLOCK_STRETCH_TIMEOUT 1000

static const char *TAG = "software_i2c";

/* https://esp-idf.readthedocs.io/en/latest/api-reference/peripherals/i2c.html#_CPPv211i2c_set_pin10i2c_port_tii13gpio_pullup_t13gpio_pullup_t10i2c_mode_t */

/* esp_err_t i2c_set_pin(i2c_port_t i2c_num, int sda_io_num, int scl_io_num, gpio_pullup_t sda_pullup_en, gpio_pullup_t scl_pullup_en, i2c_mode_t mode) */
esp_err_t sw_i2c_init(uint8_t sda, uint8_t scl)
{
    printf("Initializing software i2c with data pin %d.\n", sda);
    gpio_set_direction(sda, GPIO_MODE_INPUT_OUTPUT_OD);
    gpio_set_pull_mode(sda, GPIO_FLOATING);

    printf("Initializing software i2c with clock pin %d.\n", scl);
    gpio_set_direction(scl, GPIO_MODE_INPUT_OUTPUT_OD);
    gpio_set_pull_mode(scl, GPIO_FLOATING);

    /* Save the pins in static global variables. */
    g_i2c_sda = sda;
    g_i2c_scl = scl;
    return ESP_OK;
}

/* esp_err_t i2c_master_start(i2c_cmd_handle_t cmd_handle) */
esp_err_t sw_i2c_master_start()
{
    uint32_t stretch = CLOCK_STRETCH_TIMEOUT;

    /* If already started, do a restart condition. */
    if (g_i2c_started)
    {
        gpio_set_level(g_i2c_sda, HIGH);
        ets_delay_us(10);
        gpio_set_level(g_i2c_scl, HIGH);
        while (gpio_get_level(g_i2c_scl) == LOW && stretch--)
        {
            ets_delay_us(1);
        };
        ets_delay_us(10);
    }

    if (LOW == gpio_get_level(g_i2c_sda))
    {
        ESP_LOGD(TAG, "Arbitration lost in sw_i2c_master_start()");
    }

    /* Start bit is indicated by a high-to-low transition of SDA with SCL high. */
    gpio_set_level(g_i2c_sda, LOW);
    ets_delay_us(10);
    gpio_set_level(g_i2c_scl, LOW);

    g_i2c_started = true;

    return ESP_OK;
}

/* esp_err_t i2c_master_stop(i2c_cmd_handle_t cmd_handle) */
esp_err_t sw_i2c_master_stop()
{
    uint32_t stretch = CLOCK_STRETCH_TIMEOUT;

    /* The stop bit is indicated by a low-to-high transition of SDA with SCL high. */
    gpio_set_level(g_i2c_sda, LOW);
    ets_delay_us(10);
    gpio_set_level(g_i2c_scl, HIGH);

    while (gpio_get_level(g_i2c_scl) == LOW && stretch--)
    {
        ets_delay_us(1);
    };

    ets_delay_us(10);
    gpio_set_level(g_i2c_sda, HIGH);
    ets_delay_us(10);

    if (gpio_get_level(g_i2c_sda) == LOW)
    {
        ESP_LOGD(TAG, "Arbitration lost in sw_i2c_master_stop()");
    }

    g_i2c_started = false;

    return ESP_OK;
}

static void sw_i2c_write_bit(bool bit)
{
    uint32_t stretch = CLOCK_STRETCH_TIMEOUT;

    gpio_set_level(g_i2c_sda, bit);
    ets_delay_us(10);                /* SDA change propagation delay */
    gpio_set_level(g_i2c_scl, HIGH); /* New valid SDA value is available. */

    while (gpio_get_level(g_i2c_scl) == LOW && stretch--)
    {
        ets_delay_us(1);
    };

    ets_delay_us(10); /* Wait for SDA value to be read by slave. */

    if (bit && (LOW == gpio_get_level(g_i2c_sda)))
    {
        ESP_LOGD(TAG, "Arbitration lost in sw_i2c_write_bit()");
    }

    gpio_set_level(g_i2c_scl, LOW); /* Prepare for next bit. */
}

static bool sw_i2c_read_bit()
{
    uint32_t stretch = CLOCK_STRETCH_TIMEOUT;
    bool bit;

    gpio_set_level(g_i2c_sda, HIGH); /* Let the slave drive data. */
    ets_delay_us(10);                /* Wait for slave to write. */
    gpio_set_level(g_i2c_scl, HIGH); /* New valid SDA value is available. */

    while (gpio_get_level(g_i2c_scl) == LOW && stretch--)
    {
        ets_delay_us(1);
    };

    ets_delay_us(10);                /* Wait for slave to write. */
    bit = gpio_get_level(g_i2c_sda); /* SCL is high, read a bit. */
    gpio_set_level(g_i2c_scl, LOW);  /* Prepare for next bit. */

    return bit;
}

static uint8_t sw_i2c_read_byte(bool ack)
{
    uint8_t byte = 0;
    uint8_t bit;

    for (bit = 0; bit < 8; ++bit)
    {
        byte = (byte << 1) | sw_i2c_read_bit();
    }
    sw_i2c_write_bit(ack);

    return byte;
}

static bool sw_i2c_write_byte(uint8_t byte)
{
    uint8_t bit;
    bool ack;

    for (bit = 0; bit < 8; ++bit)
    {
        sw_i2c_write_bit((byte & 0x80) != 0);
        byte <<= 1;
    }
    ack = sw_i2c_read_bit();
    return ack;
}

/* esp_err_t i2c_master_write_byte(i2c_cmd_handle_t cmd_handle, uint8_t data, bool ack_en) */
esp_err_t sw_i2c_master_write_byte(uint8_t buffer)
{
    return sw_i2c_write_byte(buffer);
    //return ESP_OK;
}

/* esp_err_t i2c_master_write(i2c_cmd_handle_t cmd_handle, uint8_t *data, size_t data_len, bool ack_en) */
esp_err_t sw_i2c_master_write(uint8_t *buffer, uint8_t length) // bool ack_enable??
{
    while (length--)
    {
        sw_i2c_write_byte(*buffer++);
    }

    return ESP_OK;
}

/* esp_err_t i2c_master_read_byte(i2c_cmd_handle_t cmd_handle, uint8_t *data, i2c_ack_type_t ack) */
esp_err_t sw_i2c_master_read_byte(uint8_t *buffer, bool ack)
{
    *buffer = sw_i2c_read_byte(ack);
    return ESP_OK;
};

/* esp_err_t i2c_master_read(i2c_cmd_handle_t cmd_handle, uint8_t *data, size_t data_len, i2c_ack_type_t ack) */
esp_err_t sw_i2c_master_read(uint8_t *buffer, uint16_t length, bool ack)
{
    while (length)
    {
        *buffer = sw_i2c_read_byte(ack);
        buffer++;
        length--;
    }

    return ESP_OK;
}

// ******************* TOUCH FT5206 ************************
void FT5206_write_single_register(uint8_t add, uint8_t reg, uint8_t data)
{
    sw_i2c_master_start();
    sw_i2c_master_write_byte((add << 1) | I2C_MASTER_WRITE);
    sw_i2c_master_write_byte(reg);
    sw_i2c_master_write_byte(data);
    sw_i2c_master_stop();
}
void FT5206_read_registers(uint8_t add, uint8_t *registers, int num)
{
    sw_i2c_master_start();
    sw_i2c_master_write_byte((add << 1) | I2C_MASTER_READ);
    if (num > 1)
    {
        sw_i2c_master_read((uint8_t *)registers, num - 1, ACK);
    }
    sw_i2c_master_read_byte((uint8_t *)registers + num - 1, NAK);
    sw_i2c_master_stop();
}

// ****************** NETWORK ANALIZER **********************

void sw_i2c_spi_write(uint8_t DEVICE_ADDR, uint8_t cmdByte, uint8_t *data, int count)
{
    sw_i2c_master_start();
    sw_i2c_master_write_byte((DEVICE_ADDR << 1) | I2C_MASTER_WRITE);
    sw_i2c_master_write_byte(cmdByte);
    for (int i = 0; i < count; i++)
    {
        sw_i2c_master_write_byte(data[i]);
    }
    sw_i2c_master_stop();
}

void sw_i2c_spi_read(uint8_t DEVICE_ADDR, uint8_t *data_rd, int count)
{
    sw_i2c_master_start();
    sw_i2c_master_write_byte((DEVICE_ADDR << 1) | I2C_MASTER_READ);
    if (count > 1)
    {
        sw_i2c_master_read((uint8_t *)data_rd, count - 1, ACK);
    }
    sw_i2c_master_read_byte((uint8_t *)data_rd + count - 1, NAK);
    sw_i2c_master_stop();
}

//I2C
void sw_i2c_write(uint8_t DEVICE_ADDR, uint8_t *data, int count)
{
    sw_i2c_master_start();
    sw_i2c_master_write_byte((DEVICE_ADDR << 1) | I2C_MASTER_WRITE);
    for (int i = 0; i < count; i++)
    {
        sw_i2c_master_write_byte(data[i]);
    }
    sw_i2c_master_stop();
}

void sw_i2c_read(uint8_t DEVICE_ADDR, uint8_t DEVICE_REG, uint8_t *data, int count)
{
    sw_i2c_master_start();
    sw_i2c_master_write_byte((DEVICE_ADDR << 1) | I2C_MASTER_WRITE);
    sw_i2c_master_write_byte(DEVICE_REG);
    sw_i2c_master_start();
    sw_i2c_master_write_byte((DEVICE_ADDR << 1) | I2C_MASTER_READ);
    if (count > 1)
    {
        sw_i2c_master_read((uint8_t *)data, count - 1, ACK);
    }
    sw_i2c_master_read_byte((uint8_t *)data + count - 1, NAK);
    sw_i2c_master_stop();
}

esp_err_t i2c_master_probe(uint8_t address)
{
    esp_err_t result;
    sw_i2c_master_start();
    result = sw_i2c_master_write_byte((address << 1) | I2C_MASTER_WRITE);
    sw_i2c_master_stop();

    return result;
}

void sw_i2c_master_scan()
{
    ESP_LOGD(TAG, "Scanning I2C bus.");

	uint8_t address;
	esp_err_t result;

	printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");
	printf("00:         ");
	for (address = 3; address < 0x78; address++) {
		result = i2c_master_probe(address);

		if (address % 16 == 0) {
			printf("\n%.2x:", address);
		}
		if (ESP_OK == result) {
			printf(" %.2x", address);
		} else {
			printf(" --");
		}
	}
    printf("\n");
}


//I2C device ZDU0210RJX
//Write multiple Bytes
esp_err_t i2c_uart_write_ZDU0210RJX(uint8_t DEVICE_ADDR, uint8_t *data, int count, uint8_t reg)
{
	int ret;
	//printf("WRITE ZDU0210RJX I2c Data 0: %x , count : %x \n", data[0], count);
    sw_i2c_master_start();
    ret = sw_i2c_master_write_byte((DEVICE_ADDR << 1) | I2C_MASTER_WRITE);
    sw_i2c_master_write_byte(reg);
    for (int i = 0; i < count; i++)
    {
        sw_i2c_master_write_byte(data[i]);
    }
    sw_i2c_master_stop();
	return ret;
}
//Write 1 Byte
esp_err_t i2c_uart_write_ZDU0210RJX_8(uint8_t DEVICE_ADDR, uint8_t data, uint8_t reg)
{
	int ret;
    sw_i2c_master_start();
    ret = sw_i2c_master_write_byte((DEVICE_ADDR << 1) | I2C_MASTER_WRITE);
    sw_i2c_master_write_byte(reg);
    sw_i2c_master_write_byte(data);
    sw_i2c_master_stop();
	return ret;
}
//Write multiple Bytes
esp_err_t i2c_uart_write_read_ZDU0210RJX(uint8_t DEVICE_ADDR, uint8_t *dataWrite, int countWrite, uint8_t reg, uint8_t *dataRead, int countRead)
{
	int ret;
    sw_i2c_master_start();
    ret = sw_i2c_master_write_byte((DEVICE_ADDR << 1) | I2C_MASTER_WRITE);
    sw_i2c_master_write_byte(reg);
    for (int i = 0; i < countWrite; i++)
    {
        sw_i2c_master_write_byte(dataWrite[i]);
    }
    sw_i2c_master_stop();
    i2c_uart_read_ZDU0210RJX(DEVICE_ADDR,dataRead,countRead,(reg+1));

	return ret;
}

	
esp_err_t i2c_uart_read_ZDU0210RJX(uint8_t DEVICE_ADDR, uint8_t *data_rd, int  count, uint8_t reg)
{
	int ret;
    sw_i2c_master_start();
    ret = sw_i2c_master_write_byte((DEVICE_ADDR << 1) | I2C_MASTER_WRITE);
    sw_i2c_master_write_byte(reg);
    sw_i2c_master_start();
    sw_i2c_master_write_byte((DEVICE_ADDR << 1) | I2C_MASTER_READ);
    if (count > 1)
    {
        sw_i2c_master_read((uint8_t *)data_rd, count - 1, ACK);
    }
    sw_i2c_master_read_byte((uint8_t *)data_rd + count - 1, NAK);
    sw_i2c_master_stop();

	
	return ret;
}

esp_err_t i2c_uart_read_16_ZDU0210RJX(uint8_t DEVICE_ADDR, uint8_t *data_rd, uint8_t reg)
{
    int ret;
    sw_i2c_master_start();
    ret = sw_i2c_master_write_byte((DEVICE_ADDR << 1) | I2C_MASTER_WRITE);
    sw_i2c_master_write_byte(reg);
    sw_i2c_master_start();
    sw_i2c_master_write_byte((DEVICE_ADDR << 1) | I2C_MASTER_READ);
    sw_i2c_master_read_byte((uint8_t *)data_rd, ACK);
    sw_i2c_master_read_byte((uint8_t *)data_rd + 1, NAK);
    sw_i2c_master_stop();
	return ret;
}

esp_err_t i2c_uart_enable_ZDU0210RJX(uint8_t DEVICE_ADDR, uint8_t data, uint8_t reg)
{
    int ret;
    sw_i2c_master_start();
    ret = sw_i2c_master_write_byte((DEVICE_ADDR << 1) | I2C_MASTER_WRITE);
    sw_i2c_master_write_byte(reg);
    sw_i2c_master_write_byte(data);
    sw_i2c_master_stop();
	return ret;
}




uint8_t i2c_gpio_read(uint8_t DEVICE_ADDR)
{
	uint8_t data = 0;
	

    sw_i2c_master_start();sw_i2c_master_write_byte((DEVICE_ADDR << 1) | I2C_MASTER_WRITE);
    sw_i2c_master_write_byte(0x07);
    sw_i2c_master_start();
    sw_i2c_master_write_byte((DEVICE_ADDR << 1) | I2C_MASTER_READ);
    sw_i2c_master_read_byte(&data, NAK);
    sw_i2c_master_stop();
    return data;
}

/*
GPIO - STATUS 
1 = HIGH
0 = LOW
*/
esp_err_t i2c_gpio_write(uint8_t DEVICE_ADDR, uint8_t number_gpio, uint8_t status)
{
	int ret;
	int count = 2;
	uint8_t data[] = {number_gpio, status};
    sw_i2c_master_start();
    ret = sw_i2c_master_write_byte((DEVICE_ADDR << 1) | I2C_MASTER_WRITE);
    sw_i2c_master_write_byte(0x06);
    for (int i = 0; i < count; i++)
    {
        sw_i2c_master_write_byte(data[i]);
    }
    sw_i2c_master_stop();
	return ret;
}

/*
To configure the GPIO port in the gpio place '1' 
to the gpio that wants to change the current configuration 
and in the mode place '0' to set it as input and '1' 
to define it as output
*/

esp_err_t i2c_gpio_configuration(uint8_t DEVICE_ADDR, uint8_t gpio, uint8_t mode)
{
	int ret;
	int count = 2;
	uint8_t data[] = {gpio, mode};
    sw_i2c_master_start();
    ret = sw_i2c_master_write_byte((DEVICE_ADDR << 1) | I2C_MASTER_WRITE);
    sw_i2c_master_write_byte(0x08);
    sw_i2c_master_write_byte(0x01);
    for (int i = 0; i < count; i++)
    {
        sw_i2c_master_write_byte(data[i]);
    }
    sw_i2c_master_stop();
	return ret;
}


