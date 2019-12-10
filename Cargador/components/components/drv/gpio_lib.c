#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"


/*! \brief gpio_begin.
 *
 *  This function configures the GPIOs by setting them as input or output
 *  
 *  \param number     GPIO number.
 *  \param IO         Mode(1-INPUT, 0-OUTPUT, 3-INPUT/OUTPUT).
 */
void gpio_begin(int number, int IO) {
	gpio_pad_select_gpio(number);
	if (IO == 1) {
		gpio_set_direction(number, GPIO_MODE_INPUT);
	}
	else if (IO == 0)
	{
		gpio_set_direction(number, GPIO_MODE_OUTPUT);
		//gpio_set_pull_mode(number,GPIO_PULLUP_ONLY);

	}
	else if (IO == 3)
	{
		gpio_set_direction(number, GPIO_MODE_INPUT_OUTPUT);
	}
}

/*! \brief gpio_read.
 *
 *  This function read the current state of GPIO
 *  
 *  \param number     GPIO number.
 */
int gpio_read(int number) {
	return gpio_get_level(number);
}


/*! \brief gpio_write.
 *
 *  This function set the state of GPIO
 *  
 *  \param number     GPIO number.
 */
void gpio_write(int number, int value) {
	gpio_set_level(number, value);
}
