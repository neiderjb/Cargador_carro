#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "gpio_lib.h"

void gpio_begin(int number, int IO) {
	gpio_pad_select_gpio(number);
	if (IO == 1) {
		gpio_set_direction(number, GPIO_MODE_INPUT);
	}
	else if (IO == 0)
	{
		gpio_set_direction(number, GPIO_MODE_OUTPUT);
	}
	else if (IO == 3)
	{
		gpio_set_direction(number, GPIO_MODE_INPUT_OUTPUT);
	}
}

int gpio_read(int number) {
	return gpio_get_level(number);
}
void gpio_write(int number, int value) {
	gpio_set_level(number, value);
}
