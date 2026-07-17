#include "lvgl_example.h"
#include "dma.h"
#include "rs232.h"

extern osMessageQueueId_t PotentiometerHandle;

static lv_display_t * current_disp;

void HAL_DMA_TxCpltCallback(DMA_HandleTypeDef *hdma)
{
    if (hdma->Instance == DMA2_Stream0) {
        // DMA transfer complete for our display flush
        if (current_disp != NULL) {
            lv_display_flush_ready(current_disp);
        }
    }
}

void StartTask_LVGLExample(void *argument)
{
  osThreadId_t task_id = osThreadGetId();
  uint32_t last_stack_update = 0;

  lv_init();
  lv_tick_set_cb(HAL_GetTick);
  lcd_init(); 
  touch_Init();
  
  // Setup Display
  lv_display_t * disp = lv_display_create(LCD_W, LCD_H);
  static uint8_t draw_buf[LCD_W * (LCD_H / 10) * 2]  __ALIGNED(4);
  lv_display_set_buffers(disp, draw_buf, NULL, sizeof(draw_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
  lv_display_set_flush_cb(disp, my_disp_flush);

  // Register Touchpad with LVGL
  lv_indev_t * indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);   // It acts like a mouse pointer
  lv_indev_set_read_cb(indev, my_touchpad_read);     // Attach our callback

  // ---------------------------------------------------------
  // THE TEST UI: Create a clickable button
  // ---------------------------------------------------------
  lv_obj_t * screen = lv_screen_active();
  lv_obj_set_style_bg_color(screen, lv_color_hex(0x222222), LV_PART_MAIN); // Dark gray bg

    lv_obj_t * stack_box = lv_obj_create(screen);
    lv_obj_set_size(stack_box, 220, 44);
    lv_obj_align(stack_box, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_bg_color(stack_box, lv_color_hex(0x111111), LV_PART_MAIN);
    lv_obj_set_style_border_color(stack_box, lv_color_hex(0x55AAFF), LV_PART_MAIN);
    lv_obj_set_style_border_width(stack_box, 1, LV_PART_MAIN);
    lv_obj_set_style_radius(stack_box, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_all(stack_box, 8, LV_PART_MAIN);

    lv_obj_t * stack_label = lv_label_create(stack_box);
    lv_obj_center(stack_label);
    lv_label_set_text(stack_label, "Stack free: ...");

  // Create the button on the active screen
  lv_obj_t * btn = lv_button_create(screen);
  lv_obj_set_size(btn, 160, 60);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 18); // Keep it below the stack box

  // Attach our callback function to the button
  lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);

  // Create a label inside the button
  lv_obj_t * label = lv_label_create(btn);
  lv_label_set_text(label, "Touch Me!");
  lv_obj_center(label); // Center the text inside the button
  // ---------------------------------------------------------

  /* Infinite loop */
  for(;;)
  {
        uint32_t now = HAL_GetTick();
        if((now - last_stack_update) >= 250U)
        {
            uint32_t stack_free = osThreadGetStackSpace(task_id);
            lv_label_set_text_fmt(stack_label, "Stack free: %lu bytes", (unsigned long)stack_free);
            last_stack_update = now;
        }

    lv_timer_handler(); // Let LVGL process UI updates
    osDelay(5);         // Yield 5ms to other tasks
  }
  /* USER CODE END Start_GUI_Task */
}


// The LVGL display flush callback
void my_disp_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    // Cast the 8-bit map to a 16-bit map since your color depth is 16
    uint16_t * color_p = (uint16_t *)px_map;

    // 1. Set the drawing region using your existing function
    lcd_set_address(area->x1, area->y1, area->x2, area->y2);

    // 2. Calculate the number of pixels to transfer
    uint32_t width = lv_area_get_width(area);
    uint32_t height = lv_area_get_height(area);
    uint32_t total_pixels = width * height;

    // 3. Save the display pointer for the DMA interrupt
    current_disp = disp;

    // 4. Blast the pixels to the FSMC RAM
    // Since FSMC is mapped directly to memory, we can write directly to the LCD_RAM register
    HAL_DMA_Start_IT(&hdma_memtomem_dma2_stream0, (uint32_t)color_p, (uint32_t)&LCD->LCD_RAM, total_pixels);
}

// LVGL Touchpad Read Callback
void my_touchpad_read(lv_indev_t * indev, lv_indev_data_t * data) {
    // 1. Check if the screen is currently being pressed
    if(touch_IsTouched()) {
        touch_Scan(); // Read the latest coordinates via SPI

        // 2. Tell LVGL the screen is pressed and pass the coordinates
        data->state = LV_INDEV_STATE_PRESSED;
        data->point.x = touch_GetX();
        data->point.y = touch_GetY();
    } else {
        // 3. Tell LVGL the screen is released
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

// The callback function for our button
void btn_event_cb(lv_event_t * e) {
    // Get which event was triggered
    lv_event_code_t code = lv_event_get_code(e);
    
    // Get the object (the button) that triggered the event
    lv_obj_t * btn = lv_event_get_target(e);

    if(code == LV_EVENT_CLICKED) {
        // Get the first child of the button (which is our label)
        lv_obj_t * label = lv_obj_get_child(btn, 0);
        
        // Change the text to prove it worked!
        lv_label_set_text(label, "It Works!" LV_SYMBOL_OK);
        
        // Optional: Change the button's background color to green
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x00FF00), LV_PART_MAIN);
    }
}