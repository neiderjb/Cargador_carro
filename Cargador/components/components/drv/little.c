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
void my_disp_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    //printf("my_disp_flush\n");
    mat = (uint32_t)(((area->x2) - (area->x1) + 1) * ((area->y2) - (area->y1) + 1));
    printf("lvGL area to draw: %u-%u,%u-%u %u\n\r", area->x1, area->x2, area->y1, area->y2, mat);
    //drawPixels((uint16_t *)&color_map->full, mat, 0, area->y1);
    drawPixels((uint16_t *)&color_map->full, mat, area->x1, area->y1);
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

void UpdateLabelsScreen(float voltageA, float currentA, float temperature, float powerAppA, float freq, uint8_t *data)
{

    char res[20];
    printf("Update Screen-  V:%f I:%f P:%f \n", voltageA, currentA, powerAppA);
    ftoa(voltageA, res, 2);
    lv_label_set_text(labelVol, res);
    ftoa(currentA, res, 2);
    lv_label_set_text(labelCur, res);
    ftoa(powerAppA, res, 2);
    lv_label_set_text(labelPow, res);

    lv_label_set_text(labelTime, "12:00:00 PM");
}

// Reverses a string 'str' of length 'len'
void reverse(char *str, int len)
{
    int i = 0, j = len - 1, temp;
    while (i < j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

// Converts a given integer x to string str[].
// d is the number of digits required in the output.
// If d is more than the number of digits in x,
// then 0s are added at the beginning.
int intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x)
    {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }

    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';

    reverse(str, i);
    str[i] = '\0';
    return i;
}

// Converts a floating-point/double number to a string.
void ftoa(float n, char *res, int afterpoint)
{
    // Extract integer part
    int ipart = (int)n;

    // Extract floating part
    float fpart = n - (float)ipart;

    // convert integer part to string
    int i = intToStr(ipart, res, 0);

    // check for display option after point
    if (afterpoint != 0)
    {
        res[i] = '.'; // add dot

        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter
        // is needed to handle cases like 233.007
        fpart = fpart * pow(10, afterpoint);

        intToStr((int)fpart, res + i + 1, afterpoint);
    }
}
