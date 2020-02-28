#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"

#include "FT5206.h"
//#include "i2c.h"
#include "Parameters.h"
#include "gpio_lib.h"
#include "software_i2c.h"

/* 
 * @brief  Control on Leds, Set number of led and state
 * @Parameter:
 *   number = 1, led 1
 *   number = 2, led 2
 *   number = 3, led 3
 *   State = 1, ON color GREEN
 *   State = 0, OFF
 *   State = 2, ON color RED
 */
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

void getTouchOnePositions(uint16_t *touch_coordinates, char *reg)
{
    char nr_of_touches = reg[FT5206_TD_STATUS] & 0xF;
    //char nr_of_touches = reg[FT5206_TD_STATUS] & 0xF;
    touch_coordinates[0] = (((uint16_t)reg[FT5206_TOUCH1_XH] & 0x0F) << 8) | ((uint16_t)reg[FT5206_TOUCH1_XL]);
    touch_coordinates[1] = (((uint16_t)reg[FT5206_TOUCH1_YH] & 0x0F) << 8) | ((uint16_t)reg[FT5206_TOUCH1_YL]);
    printf("getTouchPositions N-touch: %c x: %d y: %d\n", nr_of_touches, touch_coordinates[0], touch_coordinates[1]);
    // return nr_of_touches;
}

// interrupts for touch detection
volatile bool newTouch = false;

void IRAM_ATTR touch_interrupt()
{
    cambio_touch_interrupt = !cambio_touch_interrupt;
    if (cambio_touch_interrupt)
    {
        gpio_set_intr_type(GPIO_INPUT_IO_1, GPIO_INTR_LOW_LEVEL);
    }
    else
    {
        gpio_set_intr_type(GPIO_INPUT_IO_1, GPIO_INTR_HIGH_LEVEL);
    }
    //printf("Interrupción");
    //if (cambio_touch_interrupt && finish_print)
    if (cambio_touch_interrupt)
    {
        xSemaphoreGive(Semaphore_control_touch);
        newTouch = true;
        //finish_print = false;
    }
}

bool touched_FT5206()
{
    if (newTouch == true)
    {
        //printf("touched_FT5206 = true");
        newTouch = false;
        return true;
    }
    else
    {
        //printf("touched_FT5206 = false");
        return false;
    }
}

void begin_FT5206()
{

    printf("Trying to initialize FT5x06 by I2C\n");
    cambio_touch_interrupt = false;
    printf("INIT config ISR \n");
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = (gpio_int_type_t)GPIO_PIN_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL_1;
    //disable pull-down mode
    io_conf.pull_down_en = (gpio_pulldown_t)0;
    //disable pull-up mode
    io_conf.pull_up_en = (gpio_pullup_t)0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
    printf("gpio_config 1 \n");

    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL_1;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    //io_conf.pull_up_en = 1;
    io_conf.pull_up_en = (gpio_pullup_t)0;
    gpio_config(&io_conf);
    printf("gpio_config 2 \n");

    //change gpio intrrupt type for one pin
    gpio_set_intr_type((gpio_num_t)GPIO_INPUT_IO_1, GPIO_INTR_HIGH_LEVEL);
    printf("gpio_set_intr_type \n");
    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    printf("gpio_install_isr_service \n");
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add((gpio_num_t)GPIO_INPUT_IO_1, touch_interrupt, (void *)GPIO_INPUT_IO_1);
    printf("gpio_isr_handler_add \n");

    printf("END config ISR \n");

    uint8_t datasend = 0;
    if (xSemaphoreTake(Semaphore_I2C, 10))
    {
        FT5206_write_single_register(FT5206_I2C_ADDRESS, FT5206_DEVICE_MODE, datasend);
        FT5206_write_single_register(FT5206_I2C_ADDRESS, FT5206_AUTO, datasend);
        xSemaphoreGive(Semaphore_I2C);
    }

    // gpio_config_t io_conf;
    // io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL_CTP_INT;
    // io_conf.intr_type = GPIO_INTR_HIGH_LEVEL;
    // io_conf.mode = GPIO_MODE_INPUT;
    // io_conf.pull_up_en = 1;
    // io_conf.pull_down_en = 0;
    // gpio_config(&io_conf);
    // printf("gpio_config_t\n");

    // gpio_set_intr_type(CTP_INT, GPIO_INTR_HIGH_LEVEL);
    // printf("gpio_set_intr_type\n");
    // gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    // printf("gpio_install_isr_service\n");
    // gpio_isr_handler_add(CTP_INT, touch_interrupt, (void *)CTP_INT);
    // printf("gpio_isr_handler_add\n");

    // uint8_t datasend = 0;
    // FT5206_write_single_register(FT5206_I2C_ADDRESS,FT5206_DEVICE_MODE,datasend);
    // FT5206_write_single_register(FT5206_I2C_ADDRESS,FT5206_AUTO,datasend);

    printf("Setup Touch done.\n");
}

void getRegisterInfo(char *registers)
{
    if (xSemaphoreTake(Semaphore_I2C, 10))
    {
        FT5206_read_registers(FT5206_I2C_ADDRESS, (uint8_t *)registers, FT5206_NUMBER_OF_REGISTERS);
        xSemaphoreGive(Semaphore_I2C);
    }
}