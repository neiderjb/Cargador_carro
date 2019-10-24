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
		vTaskDelay(1000);
	}
}

void app_main()
{
	//SPI
	cambio = false;
	gpio_begin(TOUCH_RESET, 0);
	gpio_write(TOUCH_RESET, 0);
	gpio_begin(PIN_NUM_CS, 0);
	gpio_write(PIN_NUM_CS, 1);
	spi_begin();

	//I2C config
	sw_i2c_init(PIN_SDA, PIN_SCL);
	sw_i2c_master_scan();

	while(1){
		printf("Waiting . . . .\n");
		vTaskDelay(5000);
		break;
	}

	if (!begin_RA8875(PIN_RESET_SCREEN, RA8875_800x480))
	{
		printf("RA8875 Not Found!\n");
		while (1)
		{
			vTaskDelay(100);
		}
	}

	// turn on screesn
	init_screen();

	//touch control
	Semaphore_control_touch = xSemaphoreCreateBinary();

	
	//I2C-SPI
	begin_SC18IS602B();

	//RTC
	begin_PCF85063TP();
	calibratBySeconds(0, -0.000041);

	//Grid Analyzer
	LineFreq = 5255;
	PGAGain = 42; //X4
	VoltageGain = 50306;
	CurrentGain = 40953;
	begin_M90E32AS(LineFreq, PGAGain, VoltageGain, CurrentGain, CurrentGain, CurrentGain); //

	//initialize flash memory
	nvs_flash_init();

	//Wifi Configuration
	// wifi_begin(ConfigurationObject.ssid, ConfigurationObject.password);
	wifi_begin("CEMUSA", "Ofiled@8031");

	xTaskCreate(grid_analyzer_task, "grid_analyzer_task", 4096, NULL, 5, NULL);
	xTaskCreate(Time_Task_Control, "Time_Task_Control", 2048, NULL, 1, NULL);
	// xTaskCreate(TouchControl, "controltouch", 2048, NULL, 4, NULL);
	xTaskCreatePinnedToCore(Network_Control,"Network_Control",4096, NULL,3,NULL,1);

}