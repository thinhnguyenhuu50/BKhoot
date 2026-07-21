#include "gui_app.h"
#include "ui_styles.h"
#include "cmsis_os.h"
#include "lcd.h"
#include "touch.h"
#include "dma.h"
#include "rs232.h"

static lv_display_t * current_disp;

void gui_app_init(void) {
    init_styles();
    gui_load_role_selection_screen();
}

void HAL_DMA_TxCpltCallback(DMA_HandleTypeDef *hdma)
{
    if (hdma->Instance == DMA2_Stream0) {
        if (current_disp != NULL) {
            lv_display_flush_ready(current_disp);
        }
    }
}

static void my_disp_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    uint16_t * color_p = (uint16_t *)px_map;
    lcd_set_address(area->x1, area->y1, area->x2, area->y2);
    uint32_t width = lv_area_get_width(area);
    uint32_t height = lv_area_get_height(area);
    uint32_t total_pixels = width * height;
    current_disp = disp;
    HAL_DMA_Start_IT(&hdma_memtomem_dma2_stream0, (uint32_t)color_p, (uint32_t)&LCD->LCD_RAM, total_pixels);
}

static void my_touchpad_read(lv_indev_t * indev, lv_indev_data_t * data) {
    if(touch_IsTouched()) {
        touch_Scan();
        data->state = LV_INDEV_STATE_PRESSED;
        data->point.x = touch_GetX();
        data->point.y = touch_GetY();
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

void StartTask_LVGL(void *argument) {
    lv_init();
    lv_tick_set_cb(HAL_GetTick);
    lcd_init(); 
    touch_Init();
    
    // Setup Display
    lv_display_t * disp = lv_display_create(LCD_W, LCD_H);
    uint32_t buf_size = LCD_W * (LCD_H / 10) * 2;
    uint8_t * draw_buf = (uint8_t *)pvPortMalloc(buf_size);
    if (draw_buf == NULL) {
        debug_log("Failed to allocate draw_buf!\r\n");
        vTaskDelete(NULL);
    }
    lv_display_set_buffers(disp, draw_buf, NULL, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(disp, my_disp_flush);

    // Register Touchpad
    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, my_touchpad_read);

    gui_app_init();
    debug_log("GUI initialized\r\n");
    for(;;) {
        lv_timer_handler();
        osDelay(5);
    }
}
