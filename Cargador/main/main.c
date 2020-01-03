//Driver IDF
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_freertos_hooks.h"
//Driver-Comunication
#include "drv/software_i2c.h"
#include "drv/spi_lib.h"
#include "drv/wifi_lib.h"
#include "drv/Mqtt_lib.h"

//Driver-Hardware
#include "drv/M90E32AS.h"
#include "drv/SC18IS602B.h"
#include "drv/PCF85063TP.h"
#include "drv/ZDU0210RJX.h"
#include "drv/EPLD.h"
#include "drv/gpio_lib.h"
//Library Firmware
#include "drv/Parameters.h"
#include "drv/FunctionsCC.h"
//Driver-External HW
#include "drv/phoenixcontact.h"
#include "drv/RA8875.h"
#include "drv/FT5206.h"
#include "drv/little.h"
//library LittleVgl
#include "../components/lvgl/lvgl.h"
#include "lv_examples/lv_cargador/cargador/cargador.h"
//TimerControl
#include "drv/TimerControl.h"

//LittleVgl Variables
uint16_t *buf1;
static lv_disp_buf_t disp_buf;
static void IRAM_ATTR lv_tick_task(void);

static const char *TAG = "Main";

void Network_Control(void *p)
{
	ESP_LOGI(TAG, "Initiation Network task");
	vTaskDelay(5000 / portTICK_RATE_MS);
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
			// if (ready_information)
			// {
			// ReadInformation();	//Send data to MQTT
			// 	vTaskDelay(5000 / portTICK_RATE_MS);
			// }
		}
		vTaskDelay(100 / portTICK_RATE_MS);
	}
}

void app_main()
{
	printf("----------Wait Initialize----------\n");
	vTaskDelay(20 / portTICK_RATE_MS);
	detectAnalizer = false;
	detectTouch = false;
	detectRtc = false;
	detectModbus = false;
	charging = false;

	Semaphore_control_touch = xSemaphoreCreateBinary();
	Semaphore_Start_Charging = xSemaphoreCreateBinary();
	Semaphore_Stop_Charging = xSemaphoreCreateBinary();
	Semaphore_Out_Phoenix = xSemaphoreCreateBinary();
	Semaphore_Out_Rele = xSemaphoreCreateBinary();
	Semaphore_Out_Led = xSemaphoreCreateBinary();

	//EPLD
	begin_maxV();

	//TimerControl
	timer_begin();

	/*	//Codigo solo Probar Analizador de RED 
	////Debug Analizer
	//I2C config
	sw_i2c_init(PIN_SDA, PIN_SCL);
	sw_i2c_master_scan();
	
	begin_SC18IS602B();
	//configuration analizer
	begin_analizer();
	begin_calibration_analizer(LineFreq, PGAGain, VoltageGain, CurrentGain, 60853, 63853);
	xTaskCreate(grid_analyzer_task, "grid_analyzer_task", 4096, NULL, 5, NULL);
	while (1)
	{
		vTaskDelay(100);
	}
	//Debug Analizer
 	*/

	//I2C config
	sw_i2c_init(PIN_SDA, PIN_SCL);
	sw_i2c_master_scan();

	//Touch Screen Init
	if (detectTouch)
	{
		gpio_begin(TOUCH_RESET, 0);
		gpio_write(TOUCH_RESET, 0);
		vTaskDelay(10 / portTICK_RATE_MS);
		gpio_write(TOUCH_RESET, 1);
		begin_FT5206();
	}
	//finish_print = true;

	//SPI
	spi_begin();
	//Video Screen Init
	if (!begin_RA8875(RA8875_800x480))
	{
		printf("Screen RA8875 Not Found!\n");
		while (10 / portTICK_RATE_MS)
		{
			vTaskDelay(100 / portTICK_RATE_MS);
		}
	}
	// turn on screesn
	init_screen();

	//I2C-UART
	begin_ZDU0210RJX(0xFF, 0xFF);

	//I2C-SPI
	if (detectAnalizer)
	{
		begin_SC18IS602B();
		//configuration analizer
		begin_analizer();
		begin_calibration_analizer(LineFreq, PGAGain, VoltageGain, CurrentGain, 60853, 63853);
	}

	//RTC
	if (detectRtc)
	{
		begin_PCF85063TP();
		calibratBySeconds(0, -0.000041);
	}

	//initialize flash memory
	nvs_flash_init();

	//Wifi Configuration
	// wifi_begin(ConfigurationObject.ssid, ConfigurationObject.password);
	//pwifi_begin("CEMUSA", "Ofiled@8031");
	wifi_begin("DeepSea Developments", "hexaverse"); //ISSUE cuando no tiene red falla
	if (detectAnalizer)
	{
		xTaskCreate(grid_analyzer_task, "grid_analyzer_task", 2048, NULL, 5, NULL);
	}
	if (detectModbus)
	{
		begin_phoenixcontact();
		xTaskCreate(phoenix_task, "phoenix_task", 2048, NULL, 5, NULL);
	}
	//xTaskCreate(Time_Task_Control, "Time_Task_Control", 2048, NULL, 1, NULL);
	xTaskCreatePinnedToCore(Network_Control, "Network_Control", 3072, NULL, 3, NULL, 1);

	//LittleVgl Init
	lv_init();
#ifdef littleOpt
	//buf1 = heap_caps_malloc(120000, MALLOC_CAP_DMA);
	//lv_disp_buf_init(&disp_buf, buf1, NULL, 60000);
	buf1 = heap_caps_malloc(40001, MALLOC_CAP_DMA);
	lv_disp_buf_init(&disp_buf, buf1, NULL, DISP_BUF_SIZE);
#else
	buf1 = heap_caps_malloc(40001, MALLOC_CAP_DMA);
	lv_disp_buf_init(&disp_buf, buf1, NULL, DISP_BUF_SIZE);
#endif
	//screen LittleVgl
	lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);
	disp_drv.flush_cb = my_disp_flush;
	disp_drv.buffer = &disp_buf;
	disp_drv.hor_res = 800;
	disp_drv.ver_res = 480;
	lv_disp_drv_register(&disp_drv);
	//Touch LittleVgl
	if (detectTouch)
	{
		lv_indev_drv_t indev_drv;
		lv_indev_drv_init(&indev_drv);
		indev_drv.read_cb = my_input_read;
		indev_drv.type = LV_INDEV_TYPE_POINTER;
		lv_indev_drv_register(&indev_drv);
	}
	//Screen
	esp_register_freertos_tick_hook(lv_tick_task);
	cargador_create();
	// vTaskDelay(1000);
	spi_config(true);
	//Periodic Timer
	// ESP_ERROR_CHECK(esp_timer_start_periodic(Timer_Memory_Control, 10000000));

	//Screen
	while (1)
	{
		#ifdef littleOpt
		vTaskDelay(portTICK_RATE_MS);
		#else
		vTaskDelay(1 / portTICK_RATE_MS);
		#endif
		lv_task_handler();
	}
}

//Screen
static void IRAM_ATTR lv_tick_task(void)
{
	lv_tick_inc(portTICK_RATE_MS);
}
