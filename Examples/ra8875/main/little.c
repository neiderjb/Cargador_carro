#include "little.h"

#include "RA8875.h"

void set_pixel(int32_t x, int32_t y, lv_color_t color)
{
    drawPixel((int16_t)x, (int16_t)y, color.full);
}

void my_disp_flush(lv_disp_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    int32_t x, y;
    for (y = area->y1; y <= area->y2; y++)
    {
        for (x = area->x1; x <= area->x2; x++)
        {
            set_pixel(x, y, *color_p); /* Put a pixel to the display.*/
            color_p++;
        }
    }

    lv_disp_flush_ready(disp); /* Indicate you are ready with the flushing*/
}

