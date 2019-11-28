#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_freertos_hooks.h"

#include "drv/software_i2c.h"
#include "drv/Parameters.h"
#include "drv/M90E32AS.h"
#include "drv/SC18IS602B.h"
#include "drv/PCF85063TP.h"
#include "drv/wifi_lib.h"
#include "drv/Mqtt_lib.h"
#include "drv/spi_lib.h"
#include "drv/gpio_lib.h"
#include "drv/FunctionsCC.h"
#include "drv/EPLD.h"
#include "drv/ZDU0210RJX.h"

#include "drv/RA8875.h"
#include "drv/FT5206.h"
#include "drv/little.h"

#include "../components/lvgl/lvgl.h"
#include "lv_cargador/cargador/cargador.h"

static void IRAM_ATTR lv_tick_task(void);

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
		vTaskDelay(100/ portTICK_RATE_MS);
	}
}

void app_main()
{
	
	autocs = false;
	cambio = false;
	Semaphore_control_touch = xSemaphoreCreateBinary();

	//I2C config
	sw_i2c_init(PIN_SDA, PIN_SCL);
	sw_i2c_master_scan();

	cambio = false;
	gpio_begin(TOUCH_RESET, 0);
    gpio_write(TOUCH_RESET, 0);
	vTaskDelay(10/portTICK_RATE_MS);
	gpio_write(TOUCH_RESET, 1);
	begin_FT5206();
	finish_print = true;
	
	//SPI
	spi_begin();

	//
	while (1)
	{
		printf("Waiting . . . .\n");
		vTaskDelay(500/ portTICK_RATE_MS);
		break;
	}
	
	// Init Screen
	if (!begin_RA8875(RA8875_800x480))
	{
		printf("RA8875 Not Found!\n");
		while (10 / portTICK_RATE_MS)
		{
			vTaskDelay(100);
		}
	}
	// turn on screesn
	init_screen();

	// turn on screesn
	init_screen();
	vTaskDelay(100/ portTICK_RATE_MS);
	fillScreen(RA8875_BLACK);
	vTaskDelay(100/ portTICK_RATE_MS);
	fillScreen(RA8875_WHITE);
	

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
	//pwifi_begin("CEMUSA", "Ofiled@8031");
	wifi_begin("DeepSea Developments", "hexaverse");			//ISSUE cuando no tiene red falla

	xTaskCreate(grid_analyzer_task, "grid_analyzer_task", 4096, NULL, 5, NULL);
	xTaskCreate(Time_Task_Control, "Time_Task_Control", 2048, NULL, 1, NULL);
	// xTaskCreate(TouchControl, "controltouch", 2048, NULL, 4, NULL);
	xTaskCreatePinnedToCore(Network_Control, "Network_Control", 4096, NULL, 3, NULL, 1);

	lv_init();

	static lv_disp_buf_t disp_buf;
	uint16_t *buf1;
	buf1 = heap_caps_malloc(60001, MALLOC_CAP_DMA);

	lv_disp_buf_init(&disp_buf, buf1, NULL, 800);

	//screen LittleVgl
	lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);
	disp_drv.flush_cb = my_disp_flush;
	disp_drv.buffer = &disp_buf;
	disp_drv.hor_res = 800;
	disp_drv.ver_res = 480;
	lv_disp_drv_register(&disp_drv);

	//Touch LittleVgl
	lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    //indev_drv.read_cb = xpt2046_read;
	indev_drv.read_cb = my_input_read;
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);

	//Screen
	esp_register_freertos_tick_hook(lv_tick_task);

	//demo_create();
	cargador_create();

	//Screen
	while (1)
	{
		vTaskDelay(1 / portTICK_RATE_MS);
		lv_task_handler();
	}
}

//Screen 
static void IRAM_ATTR lv_tick_task(void)
{
	lv_tick_inc(portTICK_RATE_MS);
}
