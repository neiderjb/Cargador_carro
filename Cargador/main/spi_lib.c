#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include <esp_log.h>

#include "spi_lib.h"
#include "Parameters.h"
#include "gpio_lib.h"

static const char *TAG = "SPI";

void preTransferCallback(spi_transaction_t *t)
{
	gpio_write(PIN_NUM_CS, 0);
}

void postTransferCallback(spi_transaction_t *t)
{
	gpio_write(PIN_NUM_CS, 1);
}

void sendtospi(bool ReadWrite, spi_device_handle_t spi, uint8_t command, uint8_t datatosend, uint8_t *dataread)
{
	esp_err_t ret;
	if (ReadWrite)
	{
		spi_transaction_t t;
		memset(&t, 0, sizeof(t)); //Zero out the transaction
		t.length = 16;			  //Data length 32 bits
		t.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA;
		t.tx_data[0] = command;
		t.tx_data[1] = 0x00;
		ret = spi_device_transmit(spi, &t);
		dataread[0] = t.rx_data[1];
	}
	else
	{
		spi_transaction_t t;
		memset(&t, 0, sizeof(t)); //Zero out the transaction
		t.length = 16;			  //Data length 32 bits
		t.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA;
		t.tx_data[0] = command;
		t.tx_data[1] = datatosend;
		ret = spi_device_transmit(spi, &t);
	}
}

void spi_begin()
{
	spi_bus_config_t buscfg = {
		.miso_io_num = PIN_NUM_MISO,
		.mosi_io_num = PIN_NUM_MOSI,
		.sclk_io_num = PIN_NUM_CLK,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1};

	spi_bus_initialize(VSPI_HOST, &buscfg, 0);
	// spi_bus_initialize(HSPI_HOST, &buscfg, 1);

	spi_device_interface_config_t dev_config;
	dev_config.command_bits = 0;
	dev_config.address_bits = 0;
	dev_config.dummy_bits = 0;
	dev_config.mode = 3;
	dev_config.duty_cycle_pos = 128; // default 128 = 50%/50% duty
	dev_config.cs_ena_pretrans = 0;  // 0 not used
	dev_config.cs_ena_posttrans = 0; // 0 not used
	dev_config.clock_speed_hz = 1*1000*1000;
	dev_config.spics_io_num = -1;
	dev_config.flags = SPI_DEVICE_NO_DUMMY; // 0 not used
	dev_config.queue_size = 4;
	dev_config.pre_cb = preTransferCallback;
	dev_config.post_cb = postTransferCallback;
	

	spi_bus_add_device(VSPI_HOST, &dev_config, &spi_handle);
	// spi_bus_add_device(HSPI_HOST, &dev_config, &spi_handle);
	ESP_LOGI(TAG, "begin_SPI OK");
}