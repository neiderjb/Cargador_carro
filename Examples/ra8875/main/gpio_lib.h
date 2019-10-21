#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"


void gpio_begin(int number, int IO);
int gpio_read(int number);
void gpio_write(int number, int value);
