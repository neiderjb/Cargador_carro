#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio_lib.h"
#include "EPLD.h"

#include "esp_log.h"

static const char *TAG = "EPLD";

/*Bus configuration, set all esp's inputs and esp's outputs*/
void begin_maxV()
{
    //GPIO bidirectional, R/W Bus
    gpio_begin(AD0, 3);
    gpio_begin(AD1, 3);
    gpio_begin(AD2, 3);
    gpio_begin(AD3, 3);
    //GPIO input
    gpio_begin(AD4, 1);
    gpio_begin(AD5, 1);
    gpio_begin(AD6, 1);
    gpio_begin(AD7, 1);

    //GPIO output
    gpio_begin(ALE, 0);
    gpio_begin(RW, 0);
    gpio_begin(CS, 0);

    //rele_state_maxV(1, 0); //Off Rele Power AC
    //rele_state_maxV(4, 0); //Off Rele Opto 1-2
    led_state_maxV(1, 1);

    ESP_LOGI(TAG, "begin_maxV OK");
}

/*Control on Leds, Set number of led and state (1 = on, 0 = off) */
uint8_t state_led1 = 0;
uint8_t state_led2 = 0;

void led_state_maxV(int number, int state)
{

    switch (number)
    {
    case 1:
    {
        gpio_write(RW, 1);
        gpio_write(CS, 1);
        gpio_write(ALE, 1);
        gpio_write(AD0, 0);
        gpio_write(AD1, 0);
        gpio_write(AD2, 0);
        gpio_write(AD3, 0);
        gpio_write(ALE, 0);
        vTaskDelay(1);

        if (state == 1)
        {
            state_led1 = 1;
            gpio_write(AD0, 1);
            gpio_write(AD1, 0);
            if (state_led2 == 0)
            {
                gpio_write(AD2, 0);
                gpio_write(AD3, 0);
            }
            else if (state_led2 == 1)
            {
                gpio_write(AD2, 1);
                gpio_write(AD3, 0);
            }
            else if (state_led2 == 2)
            {
                gpio_write(AD2, 0);
                gpio_write(AD3, 1);
            }
            vTaskDelay(1);
            gpio_write(RW, 0);
            gpio_write(CS, 0);
            vTaskDelay(1);
            gpio_write(RW, 1);
            gpio_write(CS, 1);
        }
        else if (state == 2)
        {
            state_led1 = 2;
            gpio_write(AD0, 0);
            gpio_write(AD1, 1);
            if (state_led2 == 0)
            {
                gpio_write(AD2, 0);
                gpio_write(AD3, 0);
            }
            else if (state_led2 == 1)
            {
                gpio_write(AD2, 1);
                gpio_write(AD3, 0);
            }
            else if (state_led2 == 2)
            {
                gpio_write(AD2, 0);
                gpio_write(AD3, 1);
            }
            vTaskDelay(1);
            gpio_write(RW, 0);
            gpio_write(CS, 0);
            vTaskDelay(1);
            gpio_write(RW, 1);
            gpio_write(CS, 1);
        }
        else
        {
            state_led1 = 0;
            gpio_write(AD0, 0);
            gpio_write(AD1, 0);
            if (state_led2 == 0)
            {
                gpio_write(AD2, 0);
                gpio_write(AD3, 0);
            }
            else if (state_led2 == 1)
            {
                gpio_write(AD2, 1);
                gpio_write(AD3, 0);
            }
            else if (state_led2 == 2)
            {
                gpio_write(AD2, 0);
                gpio_write(AD3, 1);
            }
            vTaskDelay(1);
            gpio_write(RW, 0);
            gpio_write(CS, 0);
            vTaskDelay(1);
            gpio_write(RW, 1);
            gpio_write(CS, 1);
        }
        break;
    }
    case 2:
    {
        gpio_write(RW, 1);
        gpio_write(CS, 1);
        gpio_write(ALE, 1);
        gpio_write(AD0, 0);
        gpio_write(AD1, 0);
        gpio_write(AD2, 0);
        gpio_write(AD3, 0);
        gpio_write(ALE, 0);
        vTaskDelay(1);

        if (state == 1)
        {
            state_led2 = 1;
            if (state_led1 == 0)
            {
                gpio_write(AD0, 0);
                gpio_write(AD1, 0);
            }
            else if (state_led1 == 1)
            {
                gpio_write(AD0, 1);
                gpio_write(AD1, 0);
            }
            else if (state_led1 == 2)
            {
                gpio_write(AD0, 0);
                gpio_write(AD1, 1);
            }
            gpio_write(AD2, 1);
            gpio_write(AD3, 0);
            vTaskDelay(1);
            gpio_write(RW, 0);
            gpio_write(CS, 0);
            vTaskDelay(1);
            gpio_write(RW, 1);
            gpio_write(CS, 1);
        }
        else if (state == 2)
        {
            state_led2 = 2;
            if (state_led1 == 0)
            {
                gpio_write(AD0, 0);
                gpio_write(AD1, 0);
            }
            else if (state_led1 == 1)
            {
                gpio_write(AD0, 1);
                gpio_write(AD1, 0);
            }
            else if (state_led1 == 2)
            {
                gpio_write(AD0, 0);
                gpio_write(AD1, 1);
            }
            gpio_write(AD2, 0);
            gpio_write(AD3, 1);
            vTaskDelay(1);
            gpio_write(RW, 0);
            gpio_write(CS, 0);
            vTaskDelay(1);
            gpio_write(RW, 1);
            gpio_write(CS, 1);
        }
        else
        {
            state_led2 = 0;
            if (state_led1 == 0)
            {
                gpio_write(AD0, 0);
                gpio_write(AD1, 0);
            }
            else if (state_led1 == 1)
            {
                gpio_write(AD0, 1);
                gpio_write(AD1, 0);
            }
            else if (state_led1 == 2)
            {
                gpio_write(AD0, 0);
                gpio_write(AD1, 1);
            }
            gpio_write(AD2, 0);
            gpio_write(AD3, 0);
            vTaskDelay(1);
            gpio_write(RW, 0);
            gpio_write(CS, 0);
            vTaskDelay(1);
            gpio_write(RW, 1);
            gpio_write(CS, 1);
        }
        break;
    }
    case 3:
    {
        gpio_write(RW, 1);
        gpio_write(CS, 1);
        gpio_write(ALE, 1);
        gpio_write(AD0, 1);
        gpio_write(AD1, 0);
        gpio_write(AD2, 0);
        gpio_write(AD3, 0);
        gpio_write(ALE, 0);
        vTaskDelay(1);

        if (state == 1)
        {
            gpio_write(AD0, 1);
            gpio_write(AD1, 0);
            gpio_write(AD2, 0);
            gpio_write(AD3, 0);
            vTaskDelay(1);
            gpio_write(RW, 0);
            gpio_write(CS, 0);
            vTaskDelay(1);
            gpio_write(RW, 1);
            gpio_write(CS, 1);
        }
        else if (state == 2)
        {
            gpio_write(AD0, 0);
            gpio_write(AD1, 1);
            gpio_write(AD2, 0);
            gpio_write(AD3, 0);
            vTaskDelay(1);
            gpio_write(RW, 0);
            gpio_write(CS, 0);
            vTaskDelay(1);
            gpio_write(RW, 1);
            gpio_write(CS, 1);
        }
        else
        {
            gpio_write(AD0, 0);
            gpio_write(AD1, 0);
            gpio_write(AD2, 0);
            gpio_write(AD3, 0);
            vTaskDelay(1);
            gpio_write(RW, 0);
            gpio_write(CS, 0);
            vTaskDelay(1);
            gpio_write(RW, 1);
            gpio_write(CS, 1);
        }
        break;
    }
    case 4:
    {
        gpio_write(RW, 1);
        gpio_write(CS, 1);
        gpio_write(ALE, 1);
        gpio_write(AD0, 0);
        gpio_write(AD1, 0);
        gpio_write(AD2, 0);
        gpio_write(AD3, 0);
        gpio_write(ALE, 0);
        vTaskDelay(1);

        if (state == 1)
        {
            gpio_write(AD0, 1);
            gpio_write(AD1, 0);
            gpio_write(AD2, 1);
            gpio_write(AD3, 0);
            vTaskDelay(1);
            gpio_write(RW, 0);
            gpio_write(CS, 0);
            vTaskDelay(1);
            gpio_write(RW, 1);
            gpio_write(CS, 1);
            vTaskDelay(1);

            gpio_write(ALE, 1);
            gpio_write(AD0, 1);
            gpio_write(AD1, 0);
            gpio_write(AD2, 0);
            gpio_write(AD3, 0);
            gpio_write(ALE, 0);
            vTaskDelay(1);
            gpio_write(AD0, 1);
            gpio_write(AD1, 0);
            vTaskDelay(1);
            gpio_write(RW, 0);
            gpio_write(CS, 0);
            vTaskDelay(1);
            gpio_write(RW, 1);
            gpio_write(CS, 1);
        }
        else if (state == 2)
        {
            gpio_write(AD0, 0);
            gpio_write(AD1, 1);
            gpio_write(AD2, 0);
            gpio_write(AD3, 1);
            vTaskDelay(1);
            gpio_write(RW, 0);
            gpio_write(CS, 0);
            vTaskDelay(1);
            gpio_write(RW, 1);
            gpio_write(CS, 0);
            vTaskDelay(1);

            gpio_write(ALE, 1);
            gpio_write(AD0, 1);
            gpio_write(AD1, 0);
            gpio_write(AD2, 0);
            gpio_write(AD3, 0);
            gpio_write(ALE, 0);
            vTaskDelay(1);
            gpio_write(AD0, 0);
            gpio_write(AD1, 1);
            vTaskDelay(1);
            gpio_write(CS, 0);
            gpio_write(RW, 0);
            vTaskDelay(1);
            gpio_write(RW, 1);
            gpio_write(CS, 1);
        }
        else
        {
            gpio_write(AD0, 0);
            gpio_write(AD1, 0);
            gpio_write(AD2, 0);
            gpio_write(AD3, 0);
            vTaskDelay(1);
            gpio_write(RW, 0);
            gpio_write(CS, 0);
            vTaskDelay(1);
            gpio_write(RW, 1);
            gpio_write(CS, 1);
            vTaskDelay(1);

            gpio_write(ALE, 1);
            gpio_write(AD0, 1);
            gpio_write(AD1, 0);
            gpio_write(AD2, 0);
            gpio_write(AD3, 0);
            gpio_write(ALE, 0);
            vTaskDelay(1);
            gpio_write(AD0, 0);
            gpio_write(AD1, 0);
            vTaskDelay(1);
            gpio_write(RW, 0);
            gpio_write(CS, 0);
            vTaskDelay(1);
            gpio_write(RW, 1);
            gpio_write(CS, 1);
        }
        break;
    }

    default:
    {
    }
    }
}

void esp_out_maxV(int number, int state)
{

    if (number == 0)
    {
        gpio_write(CS, 1);
        gpio_write(RW, 1);
        gpio_write(ALE, 1);
        gpio_write(AD0, 0);
        gpio_write(AD1, 1);
        gpio_write(AD2, 0);
        gpio_write(AD3, 0);
        gpio_write(ALE, 0);
        vTaskDelay(1);

        if (state == 1)
        {
            gpio_write(AD0, 1);
            vTaskDelay(1);
            gpio_write(RW, 0);
            gpio_write(CS, 0);
            vTaskDelay(1);
            gpio_write(RW, 1);
            gpio_write(CS, 1);
        }
        else
        {
            gpio_write(AD0, 0);
            vTaskDelay(1);
            gpio_write(RW, 0);
            gpio_write(CS, 0);
            vTaskDelay(1);
            gpio_write(RW, 1);
            gpio_write(CS, 1);
        }
    }
    else
    {
        if (number == 1)
        {
            gpio_write(ALE, 1);
            gpio_write(AD0, 0);
            gpio_write(AD1, 1);
            gpio_write(AD2, 0);
            gpio_write(AD3, 0);
            gpio_write(ALE, 0);
            if (state == 1)
            {
                gpio_write(AD1, 1);
                gpio_write(RW, 0);
                gpio_write(CS, 0);
                gpio_write(RW, 1);
            }
            else
            {
                gpio_write(AD1, 0);
                gpio_write(RW, 0);
                gpio_write(CS, 0);
                gpio_write(RW, 1);
            }
        }
        else
        {
            if (number == 2)
            {
                gpio_write(ALE, 1);
                gpio_write(AD0, 0);
                gpio_write(AD1, 1);
                gpio_write(AD2, 0);
                gpio_write(AD3, 0);
                gpio_write(ALE, 0);
                if (state == 1)
                {
                    gpio_write(AD2, 1);
                    gpio_write(RW, 0);
                    gpio_write(CS, 0);
                    gpio_write(RW, 1);
                }
                else
                {
                    gpio_write(AD2, 0);
                    gpio_write(RW, 0);
                    gpio_write(CS, 0);
                    gpio_write(RW, 1);
                }
            }
            else
            {
                if (number == 3)
                {
                    gpio_write(ALE, 1);
                    gpio_write(AD0, 0);
                    gpio_write(AD1, 1);
                    gpio_write(AD2, 0);
                    gpio_write(AD3, 0);
                    gpio_write(ALE, 0);
                    if (state == 1)
                    {
                        gpio_write(AD3, 1);
                        gpio_write(RW, 0);
                        gpio_write(CS, 0);
                        gpio_write(RW, 1);
                    }
                    else
                    {
                        gpio_write(AD3, 0);
                        gpio_write(RW, 0);
                        gpio_write(CS, 0);
                        gpio_write(RW, 1);
                    }
                }
            }
        }
    }
}

/*Control on Rele, Set number of rele and state (1 = active, 0 = off) */
uint8_t state_rele1 = 0;
uint8_t state_rele2 = 0;
uint8_t state_relepower = 0;

void rele_state_maxV(int number, int state)
{
    if (number == 1)
    {
        gpio_write(RW, 1);
        gpio_write(CS, 1);
        gpio_write(ALE, 1);
        gpio_write(AD0, 1);
        gpio_write(AD1, 1);
        gpio_write(AD2, 0);
        gpio_write(AD3, 0);
        gpio_write(ALE, 0);
        vTaskDelay(1);

        if (state == 0)
        {
            state_relepower = 0;
            gpio_write(AD0, 0);
            gpio_write(AD1, 0);
            vTaskDelay(1);
            gpio_write(RW, 0);
            gpio_write(CS, 0);
            vTaskDelay(1);
            gpio_write(RW, 1);
            gpio_write(CS, 1);
        }

        else if (state == 1)
        {
            state_relepower = 1;
            gpio_write(AD0, 1);
            gpio_write(AD1, 0);
            vTaskDelay(1);
            gpio_write(RW, 0);
            gpio_write(CS, 0);
            vTaskDelay(1);
            gpio_write(RW, 1);
            gpio_write(CS, 1);
            vTaskDelay(400);

            gpio_write(AD0, 0);
            gpio_write(AD1, 0);
            vTaskDelay(1);
            gpio_write(RW, 0);
            gpio_write(CS, 0);
            vTaskDelay(1);
            gpio_write(RW, 1);
            gpio_write(CS, 1);
        }
        else if (state == 2)
        {
            state_relepower = 2;
            gpio_write(AD0, 0);
            gpio_write(AD1, 1);
            vTaskDelay(1);
            gpio_write(RW, 0);
            gpio_write(CS, 0);
            vTaskDelay(1);
            gpio_write(RW, 1);
            gpio_write(CS, 1);
            vTaskDelay(400);

            gpio_write(AD0, 0);
            gpio_write(AD1, 0);
            vTaskDelay(1);
            gpio_write(RW, 0);
            gpio_write(CS, 0);
            vTaskDelay(1);
            gpio_write(RW, 1);
            gpio_write(CS, 1);
        }
    }
    else if (number == 2) //RELE Optocoplado 1
    {

        gpio_write(RW, 1);
        gpio_write(CS, 1);
        gpio_write(ALE, 1);
        gpio_write(AD0, 1);
        gpio_write(AD1, 1);
        gpio_write(AD2, 0);
        gpio_write(AD3, 0);
        gpio_write(ALE, 0);
        vTaskDelay(1);

        if (state == 1)
        {
            state_rele1 = 1;
            gpio_write(AD2, 1);
            gpio_write(AD3, 0);
            vTaskDelay(1);
            gpio_write(RW, 0);
            gpio_write(CS, 0);
            vTaskDelay(1);
            gpio_write(RW, 1);
            gpio_write(CS, 1);
        }
        else
        {
            state_rele1 = 0;
            gpio_write(AD2, 0);
            gpio_write(AD3, 0);
            vTaskDelay(1);
            gpio_write(RW, 0);
            gpio_write(CS, 0);
            vTaskDelay(1);
            gpio_write(RW, 1);
            gpio_write(CS, 1);
        }
    }
    else if (number == 3) //Rele Optocoplado 2
    {

        gpio_write(RW, 1);
        gpio_write(CS, 1);
        gpio_write(ALE, 1);
        gpio_write(AD0, 1);
        gpio_write(AD1, 1);
        gpio_write(AD2, 0);
        gpio_write(AD3, 0);
        gpio_write(ALE, 0);
        vTaskDelay(1);

        if (state == 1)
        {
            state_rele2 = 1;
            gpio_write(AD2, 0);
            gpio_write(AD3, 1);
            vTaskDelay(1);
            gpio_write(RW, 0);
            gpio_write(CS, 0);
            vTaskDelay(1);
            gpio_write(RW, 1);
            gpio_write(CS, 1);
        }
        else
        {
            state_rele2 = 0;
            gpio_write(AD2, 0);
            gpio_write(AD3, 0);
            vTaskDelay(1);
            gpio_write(RW, 0);
            gpio_write(CS, 0);
            vTaskDelay(1);
            gpio_write(RW, 1);
            gpio_write(CS, 1);
        }
    }

    else if (number == 4) //Rele Optocoplado 1 y 2
    {
        gpio_write(RW, 1);
        gpio_write(CS, 1);
        gpio_write(ALE, 1);
        gpio_write(AD0, 1);
        gpio_write(AD1, 1);
        gpio_write(AD2, 0);
        gpio_write(AD3, 0);
        gpio_write(ALE, 0);
        vTaskDelay(1);

        if (state == 1)
        {
            gpio_write(AD2, 1);
            gpio_write(AD3, 1);
            vTaskDelay(1);
            gpio_write(RW, 0);
            gpio_write(CS, 0);
            vTaskDelay(1);
            gpio_write(RW, 1);
            gpio_write(CS, 1);
        }
        else
        {
            gpio_write(AD2, 0);
            gpio_write(AD3, 0);
            vTaskDelay(1);
            gpio_write(RW, 0);
            gpio_write(CS, 0);
            vTaskDelay(1);
            gpio_write(RW, 1);
            gpio_write(CS, 1);
        }
    }
}

