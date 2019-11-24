#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"


#include "software_i2c.h"
#include "Parameters.h"
#include "M90E32AS.h"
#include "SC18IS602B.h"
#include "PCF85063TP.h"
#include "wifi_lib.h"
#include "Mqtt_lib.h"
#include "RA8875.h"
#include "spi_lib.h"
#include "FT5206.h"
#include "gpio_lib.h"
#include "FunctionsCC.h"
#include "EPLD.h"
#include "ZDU0210RJX.h"


void Network_Control(void *p)
{
	bool network_signal = false;
	for (;;)
	{
		if (!Isconnected())
		{
			continue;
		}
		else
		{
			if (!network_signal)
			{
				ResetCount();
				//configuration MQTT and BT
				mqtt_config();
				mqtt_init("mqtt://platform.agrum.co", "airis/cc/01", "airis/cc/report");
				network_signal = true;
			}
		}
		vTaskDelay(1000/ portTICK_RATE_MS);
	}
}

void app_main()
{
	//SPI
	spi_begin();

	//I2C config
	sw_i2c_init(PIN_SDA, PIN_SCL);
	sw_i2c_master_scan();

	//EPLD
	begin_maxV();
	rele_state_maxV(1, 0);
	vTaskDelay(100/ portTICK_RATE_MS);
	rele_state_maxV(1, 1);
	vTaskDelay(100/ portTICK_RATE_MS);
	rele_state_maxV(1, 2);
	vTaskDelay(100/ portTICK_RATE_MS);
	rele_state_maxV(1, 0);
	vTaskDelay(100/ portTICK_RATE_MS);
	led_state_maxV(2, 2);

	//
	while (1)
	{
		printf("Waiting . . . .\n");
		vTaskDelay(500/ portTICK_RATE_MS);
		break;
	}

	if (!begin_RA8875(RA8875_800x480))
	{
		printf("Drive RA8875 Not Found! - NO screen\n");
		while (1)
		{
			vTaskDelay(100/ portTICK_RATE_MS);
		}
	}

	// turn on screesn
	init_screen();
	uint16_t *color_px;
    color_px = heap_caps_malloc(60000, MALLOC_CAP_DMA);
    color_px[0] = 0x0700;
    for (uint32_t i = 1; i < 60000; i++)
      color_px[i] = 0x07E0;
    
    drawPixels(color_px, 60000, 0, 0); 
	heap_caps_free(color_px);
    color_px[0] = 0xF800;
    for (uint32_t i = 1; i < 60000; i++)
      color_px[i] = 0xF800;
  	drawPixels(color_px, 60000, 0, 100);
	heap_caps_free(color_px);
   



	//touch control
	Semaphore_control_touch = xSemaphoreCreateBinary();

	//I2C-SPI
	begin_SC18IS602B();
	//I2C-UART
	begin_ZDU0210RJX(0xFF, 0xFF);

	//RTC
	begin_PCF85063TP();
	calibratBySeconds(0, -0.000041);

	//configuration analizer
	begin_analizer();
	begin_calibration_analizer(LineFreq, PGAGain, VoltageGain, CurrentGain, 60853, 63853);
	set_PhaseControl();

	//initialize flash memory
	nvs_flash_init();

	//Wifi Configuration
	// wifi_begin(ConfigurationObject.ssid, ConfigurationObject.password);
	wifi_begin("CEMUSA", "Ofiled@8031");

	xTaskCreate(grid_analyzer_task, "grid_analyzer_task", 4096, NULL, 5, NULL);
	xTaskCreate(Time_Task_Control, "Time_Task_Control", 2048, NULL, 1, NULL);
	// xTaskCreate(TouchControl, "controltouch", 2048, NULL, 4, NULL);
	xTaskCreatePinnedToCore(Network_Control, "Network_Control", 4096, NULL, 3, NULL, 1);
}