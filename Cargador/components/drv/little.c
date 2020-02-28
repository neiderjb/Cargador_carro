#include "little.h"
#include "../lvgl/lvgl.h"
#include <math.h>
#include <stdio.h>

#include "RA8875.h"
#include "FT5206.h"
#include "Parameters.h"

void set_pixel_RA(int32_t x, int32_t y, uint16_t color)
{
    drawPixel((int16_t)x, (int16_t)y, color);
}

uint32_t mat;
uint32_t size, sizex, sizey;
uint32_t line_obj_y;
uint8_t color_temp_size;
void my_disp_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
//printf("my_disp_flush\n");
#ifdef littleOpt
    uint32_t mat;
    mat = (uint32_t)(((area->x2) - (area->x1) + 1) * ((area->y2) - (area->y1) + 1));
    drawPixels2((uint16_t *)&color_map->full, mat, area->x1, area->y1, area->x2, area->y2);
#else
    sizex = (uint32_t)(((area->x2) - (area->x1)) + 1);
    sizey = (uint32_t)((area->y2) - (area->y1));
    line_obj_y = 0;
    size = lv_area_get_width(area) * lv_area_get_height(area);
    mat = (uint32_t)(sizex * (sizey + 1));
    //printf("lvGL area to draw: x1:%u-x2:%u,y1:%u-y2:%u size area %u size area%u sixex %u sixey %u linea y %u\n\r", area->x1, area->x2, area->y1, area->y2, mat, size, sizex,sizey, line_obj_y);
    for (int i = 0; i <= sizey; i++) //Recorre Y
    {
        //printf("lvGL print to draw: linea %u color %u size %u x %u y %u \n\r", line_obj_y, color_map[line_obj_y].full, sizex, area->x1, ((area->y1) + i));
        drawPixels((uint16_t *)&color_map[line_obj_y].full, sizex, area->x1, ((area->y1) + i));
        line_obj_y = line_obj_y + sizex;
    }
#endif

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
        displayOn(true);
        if (touched_FT5206())
        {
            getRegisterInfo(registers);
            nr_of_touches = getTouchPositions(coordinates, registers);
            //printf("X%d=%d,Y%d=%d  X%d=%d,Y%d=%d  X%d=%d,Y%d=%d  X%d=%d,Y%d=%d  X%d=%d,Y%d=%d \n", 0, coordinates[0], 0, coordinates[1], 0, coordinates[2], 0, coordinates[3], 0, coordinates[4], 0, coordinates[5], 0, coordinates[6], 0, coordinates[7], 0, coordinates[8], 0, coordinates[9]);
            x = coordinates[0];
            y = coordinates[1];
            //last_x = x;
            //last_y = y;
            memcpy(prev_coordinates, coordinates, 20);
            //finish_print = true;
            //printRawRegisterValuesToSerial(registers);
            //serialDebugOutput(nr_of_touches, coordinates);
        }
        else
        {
            //printf("No TOUCH ");
            x = last_x;
            y = last_y;
            valid = false;
            //finish_print = true;
        }
    }
    else
    {
        //printf("No TOUCH ");
        x = last_x;
        y = last_y;
        valid = false;
        //finish_print = true;
    }

    if (x != last_x && y != last_y)
    {
        data->point.x = x;
        data->point.y = y;
        data->state = valid == false ? LV_INDEV_STATE_REL : LV_INDEV_STATE_PR;
        x = last_x;
        y = last_y;
    }

    return false;
}
