#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio_lib.h"

#define AD0 26
#define AD1 27
#define AD2 32
#define AD3 33
#define AD4 34
#define AD5 35
#define AD6 36
#define AD7 39
#define ALE 21
#define RW 22
#define CS 25
#define EPLD_IRQ0 12

void begin_maxV();
void led_state_maxV(int number, int state);
void rele_state_maxV(int number, int state);
void esp_out_maxV(int number, int state);
