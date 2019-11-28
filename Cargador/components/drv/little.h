#include "../lvgl/lvgl.h"


void set_pixel_RA(int32_t x, int32_t y, uint16_t color);
void my_disp_flush(lv_disp_drv_t * drv, const lv_area_t *area, lv_color_t *color_p);
bool my_input_read(lv_indev_drv_t * drv, lv_indev_data_t*data);
