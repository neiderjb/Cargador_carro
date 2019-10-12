#include "cJSON.h"
#include "soc/timer_group_struct.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include "driver/spi_master.h"
#include <math.h>

#include "gpio_lib.h"
//#include "driver/i2s.h"

/* */

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_event_loop.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_sleep.h"
#include "esp_spi_flash.h"
#include "esp_task_wdt.h"
#include "esp_wifi.h"

#include "http_server.h"
#include "mdns.h"

#include "i2c.h"
#include "SC18IS602B.h"
#include "M90E32AS.h"
#include "PCF85063TP.h"
#include "maxV.h"
#include "ZDU0210RJX.h"
#include "modbusMaster.h"
#include "Parameters.h"
#include "Function.h"
#include "phoenixcontact.h"
#include "sim800.h"
#include "wifi_manager.h"
#include "wifi_functions.h"
#include "Mqtt_lib.h"
#include "ota.h"
#include "Bt_lib.h"
#include "spiffs_lib.h"

#include "lwip/api.h"
#include "lwip/err.h"
#include "lwip/netdb.h"

#include "nvs_flash.h"
#include "driver/gpio.h"

#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "software_i2c.h"
#include "RA8875.h"
#include "FT5206.h"
#include "spi_lib.h"

#define DEBUG_ANALYZER

#define MAIN
//#define STATE_WIFI
//#define STATE_AP
//#define STATE_MQTT
//#define STATE_OTA
// #define STATE_GPRS
//#define STATE_BT

uint8_t topic[] = "airis/cc01/power_analizer";
#define SERIAL_DEBUG_ENABLED false
uint16_t tx, ty;
uint16_t prev_coordinates[10]; // 5 pairs of x and y
char nr_of_touches = 0;

const char *TAG = "Main";

#define TASK_RESET_PERIOD_S 5

void print_text(char *text1, char *text2)
{
	fillScreen(RA8875_BLACK);
	textMode();
	textSetCursor(10, 15);
	textTransparent(RA8875_WHITE);
	textEnlarge(1);
	char* msg1 = "Voltaje A: ";
	textWrite(msg1, strlen(msg1));
	textWrite(text1, strlen(text1));
	textSetCursor(10, 40);
	textTransparent(RA8875_WHITE);
	char* msg2 = "Corriente A: ";
	textWrite(msg2, strlen(msg2));
	textEnlarge(1);
	textWrite(text2, strlen(text2));
}

void print_circle(char num, uint16_t x, uint16_t y, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t x4, uint16_t y4)
{
	switch (num)
	{
	case 1:
		fillCircle(x, y, 30, RA8875_RED);
		break;
	case 2:
		fillCircle(x, y, 30, RA8875_RED);
		fillCircle(x1, y1, 30, RA8875_YELLOW);
		break;
	case 3:
		fillCircle(x, y, 30, RA8875_RED);
		fillCircle(x1, y1, 30, RA8875_YELLOW);
		fillCircle(x2, y2, 30, RA8875_GREEN);
		break;
	case 4:
		fillCircle(x, y, 30, RA8875_RED);
		fillCircle(x1, y1, 30, RA8875_YELLOW);
		fillCircle(x2, y2, 30, RA8875_GREEN);
		fillCircle(x3, y3, 30, RA8875_MAGENTA);
		break;
	case 5:
		fillCircle(x, y, 30, RA8875_RED);
		fillCircle(x1, y1, 30, RA8875_YELLOW);
		fillCircle(x2, y2, 30, RA8875_GREEN);
		fillCircle(x3, y3, 30, RA8875_MAGENTA);
		fillCircle(x4, y4, 30, RA8875_WHITE);
		break;

	default:
		break;
	}
}

/*! \brief This Callback for user tasks Grid Analizer
 *
 *  It is responsible for reading the values ​​of the electrical 
 *  network and sending them to the MQTT server  
 *   
 */
void grid_analyzer_task(void *arg)
{
	ESP_LOGI(TAG, "Initiation grid_analyzer_task");

	float voltageA, currentA, temperature, powerFactorA, powerA, powerReacA,
		powerAppA, freq, totalWattsA;

	// vTaskDelay(2000); //Wait run others task

	// ESP_LOGI(TAG, "TEST RELES-grid_analyzer_task");
	// rele_state_maxV(1, 1);
	// vTaskDelay(100);
	// rele_state_maxV(1, 2);

	for (;;)
	{

		unsigned short sys0 = GetSysStatus0();  //EMMState0
		unsigned short sys1 = GetSysStatus1();  //EMMState1
		unsigned short en0 = GetMeterStatus0(); //EMMIntState0
		unsigned short en1 = GetMeterStatus1(); //EMMIntState1

		printf("Sys Status: S0:0x %d, S1:0x %d \n", sys0, sys1);
		printf("Meter Status: E0:0x %d, E1:0x %d \n", en0, en1);
		vTaskDelay(5);

		//if true the MCU is not getting data from the energy meter
		if (sys0 == 65535 || sys0 == 0)
		{
			led_state_maxV(2, 2);
			ESP_LOGI(TAG, "Error: Not receiving data from energy meter - check your connections \n");
			voltageA = 0;
			currentA = 0;
			temperature = 0;
			powerFactorA = 0;
			powerA = 0;
			powerReacA = 0;
			powerAppA = 0;
			freq = 0;
			totalWattsA = (voltageA * currentA);
			//begin_M90E32AS(0, LineFreq, PGAGain, VoltageGain, CurrentGain, CurrentGain, CurrentGain);	//
		}
		else
		{
			led_state_maxV(2, 1);
			// getTime();
			voltageA = GetLineVoltageA();
			currentA = GetLineCurrentA();
			temperature = GetTemperature();
			powerFactorA = GetTotalPowerFactor();
			powerA = GetActivePowerA();
			powerReacA = GetReactivePowerA();
			powerAppA = GetApparentPowerA();
			freq = GetFrequency();
			totalWattsA = (voltageA * currentA);
			char text1[10];
			sprintf(text1, "%.2f", voltageA);
			char text2[10];
			sprintf(text2, "%.2f", currentA);
			print_text(text1, text2);

#ifdef DEBUG_ANALYZER
			ESP_LOGI(TAG, "============================== \n");
			// ESP_LOGI(TAG, "Hour %d, Minute %d, Seconds %d \n", hour, minute, second);
			ESP_LOGI(TAG, "Voltage A: %f [V] \n", voltageA);
			ESP_LOGI(TAG, "Current A: %f [A] \n", currentA);
			ESP_LOGI(TAG, "Chip Temp: %f [C] \n", temperature);
			ESP_LOGI(TAG, "Power Factor A: %f [W] \n ", powerFactorA);
			ESP_LOGI(TAG, "Active Power A: %f [W] \n", powerA);
			ESP_LOGI(TAG, "Reactive Power A: %f [Var] \n", powerReacA);
			ESP_LOGI(TAG, "Apparent Power A: %f [VA] \n", powerAppA);
			ESP_LOGI(TAG, "Power total A MAT : %f [W] \n ", totalWattsA);
			ESP_LOGI(TAG, "Frequency: %f [Hz] \n", freq);
			ESP_LOGI(TAG, "============================== \n");
#else
			ESP_LOGI(TAG, "============================== \n");
			ESP_LOGI(TAG, "Grid Analyzer Debug OFF\n");
			ESP_LOGI(TAG, "============================== \n");
#endif
			char *dataMQTT = createjson(voltageA, currentA, powerFactorA, powerA, powerReacA, powerAppA, totalWattsA, temperature);

#ifdef STATE_MQTT
			if (Isconnected_wifi())
			{
				led_state_maxV(3, 1);
				sendMessage(dataMQTT, (char *)topic);
				ESP_LOGI(TAG, "Send data to Wifi \n");
				led_state_maxV(3, 0);
			}
			else
			{
				ESP_LOGE(TAG, "No connected to WIFI - No data send to broker \n");
			}
#endif

#ifdef STATE_GPRS
			if (Isconnected_gprs())
			{
				led_state_maxV(3, 1);
				sim800_postMQTT(topic, sizeof(topic), (uint8_t *)dataMQTT, strlen(dataMQTT));
				ESP_LOGI(TAG, "Send data to GPRS \n");
				led_state_maxV(3, 0);
			}
			else
			{
				ESP_LOGE(TAG, "No connected to  GPRS- No data send to broker \n");
			}
#endif
		}

		vTaskDelay(pdMS_TO_TICKS(TASK_RESET_PERIOD_S * 1000));
	}
}

/*! \brief This Callback for user tasks Phoenix Contact
 *
 * 
 */
void phoenix_contact_task(void *arg)
{
	ESP_LOGI(TAG, "Initiation phoenix_contact_task \n");

	int dataMQTTQueue[2] = {-1, -1};

	for (;;)
	{
		//Wait MQTT Command
		if (xSemaphoreTake(Semaphore_control, 10))
		{
			printf("Enter MQTT Command");
			for (int i = 0; i < 1; i++)
			{
				if (xQueueReceive(Queue_control, &dataMQTTQueue[i], 100 / portTICK_PERIOD_MS))
				{
					printf("Received MQTT Number: %d, Value: %d\n", i, dataMQTTQueue[i]);
				}
				else
				{
					ESP_LOGI(TAG, "Error received MQTT\n");
					break;
				}
			}
			xQueueReset(Queue_control);
		}

		if (dataMQTTQueue[0] == 0)
		{
			phoenixcontact_Digital_OutputBehaviorOut0(0);
			dataMQTTQueue[0] = -1;
		}
		else if (dataMQTTQueue[0] == 1)
		{
			phoenixcontact_Digital_OutputBehaviorOut0(1);
			dataMQTTQueue[0] = -1;
		}
		else if (dataMQTTQueue[0] == 2)
		{
			phoenixcontact_Digital_OutputBehaviorOut0(2);
			dataMQTTQueue[0] = -1;
		}

		vTaskDelay(pdMS_TO_TICKS(TASK_RESET_PERIOD_S * 1000));
	}
}

void network_task(void *arg)
{
	ESP_LOGI(TAG, "Initiation network task \n");

#ifdef STATE_WIFI
	ESP_LOGI(TAG, "Connecting WIFI\n");
	wifi_initialise(WIFI_SSID, WIFI_PASS);
	vTaskDelay(1000);
#endif

#ifdef STATE_GPRS
	ESP_LOGI(TAG, "Connecting gprs\n");
	begin_800();
	startCon();
	subtopic();
#endif

	if (!Isconnected_wifi())
	{
#ifdef STATE_AP
		led_state_maxV(1, 2);
		ESP_LOGI(TAG, "No connecting WIFI- Open AP WIFI\n");
		//disable the default wifi logging
		esp_log_level_set("wifi", ESP_LOG_NONE);
		//start the HTTP Server task
		xTaskCreate(&http_server, "http_server", 4096, NULL, 5, &task_http_server); //4096
		//start the wifi manager task
		xTaskCreate(&wifi_manager, "wifi_manager", 4096, NULL, 4, &task_wifi_manager); //4096
#endif
	}
	else
	{
		led_state_maxV(1, 1);
		ESP_LOGI(TAG, "Connected WIFI- Open BT- Run OTA- Run MQTT\n");
		ResetCount();
//configuration MQTT and BT
#ifdef STATE_MQTT
		mqtt_config();
		mqtt_init(config_network.broker_mqtt, config_network.Input_topic, config_network.Output_topic);
#endif

#ifdef STATE_BT
		bt_config(BT_NAME);
		bt_init();
#endif

#ifdef STATE_OTA
		begin_ota();
#endif
	}

	for (;;)
	{
		//phoenixcontact_write();
		//phoenixcontact_Set_Reset(1);
		//phoenixcontact_Digital_OutputBehaviorOut0(state_led);

		vTaskDelay(pdMS_TO_TICKS(TASK_RESET_PERIOD_S * 1000));
	}
}

void touchControl(void *p)
{

	gpio_write(15, 1);
	begin_FT5206();
	finish_print = true;
	printf("Waiting for touch events ...\n");
	for (;;)
	{
		char registers[FT5206_NUMBER_OF_REGISTERS];

		uint16_t coordinates[10];

		char prev_nr_of_touches = 0;
		if (xSemaphoreTake(Semaphore_control_touch, 10))
		{
			if (touched_FT5206())
			{
				getRegisterInfo(registers);
				nr_of_touches = getTouchPositions(coordinates, registers);
				prev_nr_of_touches = nr_of_touches;
				printf("X%d=%d,Y%d=%d  X%d=%d,Y%d=%d  X%d=%d,Y%d=%d  X%d=%d,Y%d=%d  X%d=%d,Y%d=%d \n", 0, coordinates[0], 0, coordinates[1], 0, coordinates[2], 0, coordinates[3], 0, coordinates[4], 0, coordinates[5], 0, coordinates[6], 0, coordinates[7], 0, coordinates[8], 0, coordinates[9]);

				print_circle(nr_of_touches, coordinates[0], coordinates[1], coordinates[2], coordinates[3], coordinates[4], coordinates[5], coordinates[6], coordinates[7], coordinates[8], coordinates[9]);
				vTaskDelay(10);
				memcpy(prev_coordinates, coordinates, 20);
			}
			finish_print = true;
			xSemaphoreGive(Semaphore_control_screen);
		}
		vTaskDelay(10);
	}
}

void app_main()
{

	ESP_LOGI(TAG, ">Initialize Comunication\n");

	//I2C config
	sw_i2c_init(PIN_SDA, PIN_SCL);
	sw_i2c_master_scan();
// i2c_configuration(I2C_NUM_0, PIN_SDA, PIN_SCL);
// i2c_list_devices();

// vTaskDelay(1000);
#ifdef MAIN
	ESP_LOGI(TAG, ">Initialize Devices to Board\n");
	//I2C-SPI
	begin_SC18IS602B();
	//Analize
	LineFreq = 5255;
	PGAGain = 42; //X4
	VoltageGain = 49152;
	CurrentGain = 45200;
	begin_M90E32AS(LineFreq, PGAGain, VoltageGain, CurrentGain, CurrentGain, CurrentGain); //
	//RTC
	// begin_PCF85063TP();
	// calibratBySeconds(0, -0.000041);
	//maxV
	// begin_maxV();
	// rele_state_maxV(1, 0);
	//led_state_maxV(7, 0);
	//Uart - Gpio
	// begin_ZDU0210RJX(0xFF, 0xFF);
	//Modbus comunication
	// begin_modbusMaster(3);

	// begin_phoenixcontact();
	// phoenixcontact_Digital_OutputBehaviorOut0(2);

	ESP_LOGI(TAG, ">Initialize Queue and Semaphore\n");
	Semaphore_control = xSemaphoreCreateBinary();
	Semaphore_control_timer = xSemaphoreCreateBinary();
	Queue_control = xQueueCreate(NUMBER_COMMANDS_QUEUE, sizeof(int));

	ESP_LOGI(TAG, ">Initialize config memory\n");
	//get/set default configuration
	// begin_spiffs();
	//initialize flash memory wifi and bluettoth
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	ESP_LOGI(TAG, ">Initialize Screen configuration \n");
	cambio = false;
	gpio_begin(15, 0);
	gpio_write(15, 0);

	Semaphore_control_touch = xSemaphoreCreateBinary();
	Semaphore_control_screen = xSemaphoreCreateBinary();

	// SPI Configuration
	gpio_begin(PIN_NUM_CS, 0);
	gpio_write(PIN_NUM_CS, 1);
	spi_begin();
	vTaskDelay(1000);
	while (!begin_RA8875(13, RA8875_800x480))
	{
		printf("RA8875 Not Found!\n");
		vTaskDelay(100);
	}

	printf("Found RA8875\n");
	displayOn(true);
	GPIOX(true);							  // Enable TFT - display enable tied to GPIOX
	PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
	PWM1out(255);
	// With hardware accelleration this is
	// fillScreen(RA8875_WHITE);
	vTaskDelay(500);
	fillScreen(RA8875_BLACK);
	vTaskDelay(500);
	// textMode();
	// textSetCursor(10, 10);
	// textTransparent(RA8875_WHITE);
	// textEnlarge(1);
	// char *msg = "Capacitive touch sensor demo. Touch me !";
	// textWrite(msg, strlen(msg));

	// ESP_LOGI(TAG, ">Initialize Timer and the Task \n");
	// xTaskCreate(network_task, "wifi_task", 4096, NULL, 3, &task_network);

	xTaskCreate(grid_analyzer_task, "grid_analyzer_task", 4096, NULL, 5, &task_analyzer);
	xTaskCreate(touchControl, "controltouch", 2048, NULL, 4, NULL);
// xTaskCreate(phoenix_contact_task, "phoenix_contact_task", 4096, NULL, 5, &task_phoenix);
#endif
}
