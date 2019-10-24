#include "driver/gpio.h"
#include "FT5206.h"
#include "Parameters.h"
#include "gpio_lib.h"
#include "software_i2c.h"

char getTouchPositions(uint16_t *touch_coordinates, char *reg)
{

    char nr_of_touches = reg[FT5206_TD_STATUS] & 0xF;
    if (nr_of_touches > 0)
    {
        touch_coordinates[0] = (((uint16_t)reg[FT5206_TOUCH1_XH] & 0x0F) << 8) | ((uint16_t)reg[FT5206_TOUCH1_XL]);
        touch_coordinates[1] = (((uint16_t)reg[FT5206_TOUCH1_YH] & 0x0F) << 8) | ((uint16_t)reg[FT5206_TOUCH1_YL]);
    }
    if (nr_of_touches > 1)
    {
        touch_coordinates[2] = (((uint16_t)reg[FT5206_TOUCH2_XH] & 0x0F) << 8) | ((uint16_t)reg[FT5206_TOUCH2_XL]);
        touch_coordinates[3] = (((uint16_t)reg[FT5206_TOUCH2_YH] & 0x0F) << 8) | ((uint16_t)reg[FT5206_TOUCH2_YL]);
    }
    if (nr_of_touches > 2)
    {
        touch_coordinates[4] = (((uint16_t)reg[FT5206_TOUCH3_XH] & 0x0F) << 8) | ((uint16_t)reg[FT5206_TOUCH3_XL]);
        touch_coordinates[5] = (((uint16_t)reg[FT5206_TOUCH3_YH] & 0x0F) << 8) | ((uint16_t)reg[FT5206_TOUCH3_YL]);
    }
    if (nr_of_touches > 3)
    {
        touch_coordinates[6] = (((uint16_t)reg[FT5206_TOUCH4_XH] & 0x0F) << 8) | ((uint16_t)reg[FT5206_TOUCH4_XL]);
        touch_coordinates[7] = (((uint16_t)reg[FT5206_TOUCH4_YH] & 0x0F) << 8) | ((uint16_t)reg[FT5206_TOUCH4_YL]);
    }
    if (nr_of_touches > 4)
    {
        touch_coordinates[8] = (((uint16_t)reg[FT5206_TOUCH5_XH] & 0x0F) << 8) | ((uint16_t)reg[FT5206_TOUCH5_XL]);
        touch_coordinates[9] = (((uint16_t)reg[FT5206_TOUCH5_YH] & 0x0F) << 8) | ((uint16_t)reg[FT5206_TOUCH5_YL]);
    }
    return nr_of_touches;
}

// interrupts for touch detection
volatile bool newTouch = false;

void touch_interrupt()
{
    cambio = !cambio;
    if (cambio)
    {
        gpio_set_intr_type(CTP_INT, GPIO_INTR_LOW_LEVEL);
    }
    else
    {
        gpio_set_intr_type(CTP_INT, GPIO_INTR_HIGH_LEVEL);
    }
    if (cambio && finish_print)
    {
        xSemaphoreGive(Semaphore_control_touch);
        newTouch = true;
        finish_print = false;
    }
}

bool touched_FT5206()
{
    if (newTouch == true)
    {
        newTouch = false;
        return true;
    }
    else
    {
        return false;
    }
}

void begin_FT5206()
{

    printf("Trying to initialize FT5x06 by I2C\n");

    gpio_config_t io_conf;
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL_CTP_INT;
    io_conf.intr_type = GPIO_INTR_HIGH_LEVEL;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 1;
    io_conf.pull_down_en = 0;
    gpio_config(&io_conf);

    gpio_set_intr_type(CTP_INT, GPIO_INTR_HIGH_LEVEL);
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(CTP_INT, touch_interrupt, (void *)CTP_INT);

    uint8_t datasend = 0;
    FT5206_write_single_register(FT5206_I2C_ADDRESS, FT5206_DEVICE_MODE, datasend);
    FT5206_write_single_register(FT5206_I2C_ADDRESS, FT5206_AUTO, datasend);

    printf("Setup done.\n");
}

void getRegisterInfo(char *registers)
{
    FT5206_read_registers(FT5206_I2C_ADDRESS, (uint8_t *)registers, FT5206_NUMBER_OF_REGISTERS);
}

void TouchControl(void *p)
{
    gpio_write(TOUCH_RESET, 1);
    begin_FT5206();
    finish_print = true;
    printf("Waiting for touch events ...\n");
    char nr_of_touches = 0;
    char registers[FT5206_NUMBER_OF_REGISTERS];
    uint16_t coordinates[10];

    for (;;)
    {
        if (xSemaphoreTake(Semaphore_control_touch, 10))
        {
            finish_print = false;
            if (touched_FT5206())
            {
                getRegisterInfo(registers);
                nr_of_touches = getTouchPositions(coordinates, registers);
                printf("X%d=%d,Y%d=%d  X%d=%d,Y%d=%d  X%d=%d,Y%d=%d  X%d=%d,Y%d=%d  X%d=%d,Y%d=%d \n", 0, coordinates[0], 0, coordinates[1], 0, coordinates[2], 0, coordinates[3], 0, coordinates[4], 0, coordinates[5], 0, coordinates[6], 0, coordinates[7], 0, coordinates[8], 0, coordinates[9]);

                // print_circle(nr_of_touches, coordinates[0], coordinates[1], coordinates[2], coordinates[3], coordinates[4], coordinates[5], coordinates[6], coordinates[7], coordinates[8], coordinates[9]);
                vTaskDelay(10);
                // memcpy(prev_coordinates, coordinates, 20);
            }
            finish_print = true;
        }
        vTaskDelay(100);
    }
}
