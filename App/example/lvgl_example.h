#include "lvgl.h"
#include "lcd.h"
#include "touch.h"
#include "cmsis_os.h"
#include "task.h"

void my_disp_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);
void my_touchpad_read(lv_indev_t * indev, lv_indev_data_t * data);
void btn_event_cb(lv_event_t * e);