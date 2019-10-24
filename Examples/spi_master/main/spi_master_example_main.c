#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include "driver/timer.h"

#include "gpio_lib.h"
#include "spi_lib.h"
#include "Parameters.h"
#include "RA8875.h"
#include "FT5206.h"
#include "i2c.h"
#include "software_i2c.h"

#define SERIAL_DEBUG_ENABLED false

uint16_t tx, ty;
uint16_t prev_coordinates[10]; // 5 pairs of x and y
char nr_of_touches = 0;

void serialDebugOutput(int nr_of_touches, uint16_t *coordinates)
{
	for (char i = 0; i < nr_of_touches; i++)
	{

		uint16_t x = coordinates[i * 2];
		uint16_t y = coordinates[i * 2 + 1];

		printf("X%d=%d,Y%d=%d \n", i, x, i, y);
	}
}

void printRawRegisterValuesToSerial(char *registers)
{
	// print raw register values
	for (int i = 0; i < FT5206_NUMBER_OF_REGISTERS; i++)
	{
		printf("%X, ", registers[i]);
	}
	printf("\n");
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

void ControlColor(void *p)
{

	displayOn(true);
	GPIOX(true);							  // Enable TFT - display enable tied to GPIOX
	PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
	PWM1out(255);
	// With hardware accelleration this is
	fillScreen(RA8875_WHITE);
	vTaskDelay(500);

	for (uint8_t i = 255; i != 0; i -= 5)
	{
		PWM1out(i);
		vTaskDelay(10);
	}
	for (uint8_t i = 0; i != 255; i += 5)
	{
		PWM1out(i);
		vTaskDelay(10);
	}
	PWM1out(255);
	vTaskDelay(500);
	fillScreen(RA8875_RED);
	vTaskDelay(500);
	fillScreen(RA8875_YELLOW);
	vTaskDelay(500);
	fillScreen(RA8875_GREEN);
	vTaskDelay(500);
	fillScreen(RA8875_CYAN);
	vTaskDelay(500);
	fillScreen(RA8875_MAGENTA);
	vTaskDelay(500);
	fillScreen(RA8875_BLACK);
	vTaskDelay(500);
	for (;;)
	{

		// if (xSemaphoreTake(Semaphore_control_touch, 10)){
		// 	if (touched_FT5206()){
		// 		getRegisterInfo(registers);
		// 		nr_of_touches = getTouchPositions(coordinates, registers);
		// 		prev_nr_of_touches = nr_of_touches;

		// 		if (SERIAL_DEBUG_ENABLED){
		// 			printRawRegisterValuesToSerial(registers);
		// 			serialDebugOutput(nr_of_touches, coordinates);
		// 		}

		// 		// remove previous touches on screen
		// 		for (int i = 0 ; i < nr_of_touches; i++){
		// 			uint16_t x = prev_coordinates[i * 2];
		// 			uint16_t y = prev_coordinates[i * 2 + 1];
		// 			fillCircle(x, y, 70, RA8875_GREEN);
		// 		}

		// 		// mark touches on screen
		// 		for (char i = 0; i < nr_of_touches; i++){
		// 			uint16_t x = coordinates[i * 2];
		// 			uint16_t y = coordinates[i * 2 + 1];

		// 			// Mark touches on screen
		// 			fillCircle(x, y, 70, RA8875_BLUE);
		// 			fillCircle(x, y, 50, RA8875_WHITE);
		// 			fillCircle(x, y, 30, RA8875_RED);
		// 		}
		// 		vTaskDelay(10);
		// 		memcpy(prev_coordinates, coordinates, 20);
		// 	}
		// 	finish_print = true;
		// }
		// if (xSemaphoreTake(Semaphore_control_screen, 10))
		// {
		// 	for (int i = 0; i < 750; i += 5)
		// 	{
		// 		// Mark touches on screen
		// 		fillCircle(i, 200, 70, RA8875_BLUE);
		// 		// fillCircle(i, 200, 50, RA8875_WHITE);
		// 		// fillCircle(i, 200, 30, RA8875_RED);
		// 		vTaskDelay(100);
		// 		fillScreen(RA8875_BLACK);
		// 	}

		// 	xSemaphoreGive(Semaphore_control_touch);
		// }
		vTaskDelay(1000);
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
				if (SERIAL_DEBUG_ENABLED)
				{
					printRawRegisterValuesToSerial(registers);
					serialDebugOutput(nr_of_touches, coordinates);
				}
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
	sw_i2c_init(PIN_SDA, PIN_SCL);
	sw_i2c_master_scan();
	// i2c
	// i2c_configuration(I2C_NUM_0, PIN_SDA, PIN_SCL);
	// i2c_list_devices();

	cambio = false;
	gpio_begin(15, 0);
	gpio_write(15, 0);

	Semaphore_control_touch = xSemaphoreCreateBinary();
	Semaphore_control_screen = xSemaphoreCreateBinary();

	// SPI Configuration
	gpio_begin(PIN_NUM_CS, 0);
	gpio_write(PIN_NUM_CS, 1);
	spi_begin();

	//commands
	if (!begin_RA8875(14, RA8875_800x480))
	{
		printf("RA8875 Not Found!\n");
		while (1)
		{
			vTaskDelay(100);
		}
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
	textMode();
	textSetCursor(10, 10);
	textTransparent(RA8875_WHITE);
	textEnlarge(1);
	char* msg = "Capacitive touch sensor demo. Touch me !";
	textWrite(msg,strlen(msg));
	

	// xTaskCreate(ControlColor, "control", 4096, NULL, 3, NULL);
	xTaskCreate(touchControl, "controltouch", 2048, NULL, 4, NULL);
}