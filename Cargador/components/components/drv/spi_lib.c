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


void send8(uint8_t d) 
{
   esp_err_t ret;
  spi_transaction_t t;
  memset(&t, 0, sizeof(t));       //Zero out the transaction
  t.length = 8 ;               //Command is 8 bits 
  t.tx_data[0] = (uint8_t)d;
  t.flags = SPI_TRANS_USE_TXDATA;
  t.user = (void *)0;                         //D/C needs to be set to 0
  ret = spi_device_polling_transmit(spi_handle, &t); //Transmit!
  assert(ret == ESP_OK);        //Should have had no issues.
}

uint8_t read8(void) 
{
  spi_transaction_t t;
  memset(&t, 0, sizeof(t));       //Zero out the transaction
  t.length = 8 ;               //Command is 8 bits 
  t.tx_data[0] = (uint8_t)0;
  t.flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA;
  t.user = (void *)1;

  esp_err_t ret = spi_device_polling_transmit(spi_handle, &t);
  assert(ret == ESP_OK);
  return (uint8_t)t.rx_data[0];
}

void spidrawpixels(uint16_t *p, uint32_t num){
  esp_err_t ret;
  static spi_transaction_t t;
  t.tx_buffer = p;     //finally send the line data
  t.length = 16 * num; //Data length, in bits
  t.flags = 0;
  ret = spi_device_queue_trans(spi_handle, &t, portMAX_DELAY); //spi_device_polling_transmit(spi, &t);  //Transmit!
  assert(ret == ESP_OK);                                //Should have had no issues.
  spi_transaction_t *rtrans;
  ret = spi_device_get_trans_result(spi_handle, &rtrans, portMAX_DELAY);
  assert(ret == ESP_OK);
}

void spidrawpixels2(uint16_t *p, uint32_t num, uint8_t command){
  esp_err_t ret;
  static spi_transaction_t t;
  	memset(&t, 0, sizeof(t)); 
	t.length=8;                     //Command is 8 bits
  	t.tx_data[0]=command ;	//RA8875_DATAWRITE;               //The data is the cmd itself
  	t.flags = SPI_TRANS_USE_TXDATA;
  	t.user=(void*)0;                //D/C needs to be set to 0
  	ret=spi_device_polling_transmit(spi_handle, &t);  //Transmit!
  	assert(ret==ESP_OK);            //Should have had no issues.

	while (num--) {
  	        //SPI.transfer16(*p++);
  		memset(&t, 0, sizeof(t));       //Zero out the transaction
  		t.length=16;                     //Command is 8 bits
  		t.tx_data[1]=(*p)>>8;               //The data is the cmd itself
  		t.tx_data[0]=(*p++)&0x00FF;
  		t.flags = SPI_TRANS_USE_TXDATA;
  		t.user=(void*)0;                //D/C needs to be set to 0
  		ret=spi_device_polling_transmit(spi_handle, &t);  //Transmit!
  		assert(ret==ESP_OK);
  		//vTaskDelay(10 / portTICK_RATE_MS);

  	}
}

void spi_begin()
{
	esp_err_t ret;

	spi_bus_config_t buscfg = {
		.miso_io_num = PIN_NUM_MISO ,
		.mosi_io_num = PIN_NUM_MOSI,
		.sclk_io_num = PIN_NUM_CLK,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1,
		.max_transfer_sz = 480000};
	spi_device_interface_config_t devcfg = {
	#ifdef CONFIG_LCD_OVERCLOCK
		.clock_speed_hz = 1 * 120 * 100, 
	#else
		.clock_speed_hz = 10 * 1000 * 1000, //Clock out at 10 MHz
	#endif
		.mode = 0,		   //SPI mode 0
		.spics_io_num = -1, //5 PIN_NUM_CS,               //CS pin
		.queue_size = 7,   //We want to be able to queue 7 transactions at a time
		//.pre_cb = preTransferCallback,
		//.post_cb = postTransferCallback,
		//.pre_cb=lcd_spi_pre_transfer_callback,  //Specify pre-transfer callback to handle D/C line
	};
	//Initialize the SPI bus
	ret = spi_bus_initialize(LCD_HOST, &buscfg, DMA_CHAN);
	ESP_ERROR_CHECK(ret);
	//Attach the LCD to the SPI bus
	ret = spi_bus_add_device(LCD_HOST, &devcfg, &spi_handle);
	ESP_ERROR_CHECK(ret);

	ESP_LOGI(TAG, "begin_SPI OK");
}

void spi_config()
{
	spi_device_interface_config_t devcfg = {
	#ifdef CONFIG_LCD_OVERCLOCK
		.clock_speed_hz = 20 * 1000 * 1000, //Clock out at 26 MHz
	#else
		.clock_speed_hz = 5 * 1000 * 1000, //Clock out at 10 MHz
	#endif
		.mode = 0,		   //SPI mode 0
		.spics_io_num = -1, //5 PIN_NUM_CS,               //CS pin
		.queue_size = 7,   //We want to be able to queue 7 transactions at a time
						   //.pre_cb=lcd_spi_pre_transfer_callback,  //Specify pre-transfer callback to handle D/C line
	};
	//Attach the LCD to the SPI bus
	esp_err_t ret;
	ret = spi_bus_add_device(LCD_HOST, &devcfg, &spi_handle);
	ESP_ERROR_CHECK(ret);
}