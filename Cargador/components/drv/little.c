#include "little.h"
#include "../lvgl/lvgl.h"

#include "RA8875.h"
#include "FT5206.h"
#include "Parameters.h"

void set_pixel_RA(int32_t x, int32_t y, uint16_t color)
{
    drawPixel((int16_t)x, (int16_t)y, color);
}

uint32_t mat;
void my_disp_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    //printf("my_disp_flush\n");
    mat = (uint32_t)(((area->x2) - (area->x1) + 1) * ((area->y2) - (area->y1) + 1));
    //printf("lvGL area to draw: %u-%u,%u-%u %u\n\r",area->x1,area->x2,area->y1,area->y2,mat);
    drawPixels((uint16_t *)&color_map->full, mat, 0, area->y1);
    lv_disp_flush_ready(drv); /* Indicate you are ready with the flushing*/
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

void serialDebugOutput(int nr_of_touches, uint16_t *coordinates)
{
    for (char i = 0; i < nr_of_touches; i++)
    {

        uint16_t x = coordinates[i * 2];
        uint16_t y = coordinates[i * 2 + 1];

        printf("X%d=%d,Y%d=%d \n", i, x, i, y);
    }
}

char registers[FT5206_NUMBER_OF_REGISTERS];
char nr_of_touches = 0;
uint16_t prev_coordinates[10]; // 5 pairs of x and y
uint16_t coordinates[10];

bool my_input_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    //printf("TOUCH detected");
    static int16_t last_x = 0;
    static int16_t last_y = 0;
    uint16_t x = 0;
    uint16_t y = 0;

    bool valid = true;
    if (xSemaphoreTake(Semaphore_control_touch, 10))
    {
        if (touched_FT5206())
        {
            getRegisterInfo(registers);
            nr_of_touches = getTouchPositions(coordinates, registers);
            printf("X%d=%d,Y%d=%d  X%d=%d,Y%d=%d  X%d=%d,Y%d=%d  X%d=%d,Y%d=%d  X%d=%d,Y%d=%d \n", 0, coordinates[0], 0, coordinates[1], 0, coordinates[2], 0, coordinates[3], 0, coordinates[4], 0, coordinates[5], 0, coordinates[6], 0, coordinates[7], 0, coordinates[8], 0, coordinates[9]);

            x = coordinates[0];
            y = coordinates[1];
            printf("TOUCH coordinates X:%d , Y:%d", x, y);
            last_x = x;
            last_y = y;
            memcpy(prev_coordinates, coordinates, 20);
            finish_print = true;

            printRawRegisterValuesToSerial(registers);
            serialDebugOutput(nr_of_touches, coordinates);
        }
        else
        {
            //printf("No TOUCH ");
            x = last_x;
            y = last_y;
            valid = false;
            finish_print = true;
        }
    }
    else
    {
        printf("No TOUCH ");
        x = last_x;
        y = last_y;
        valid = false;
        finish_print = true;
    }

    data->point.x = x;
    data->point.y = y;
    data->state = valid == false ? LV_INDEV_STATE_REL : LV_INDEV_STATE_PR;
    return false;
}
