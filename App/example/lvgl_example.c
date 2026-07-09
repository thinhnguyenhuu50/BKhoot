#include "lvgl_example.h"

extern osMessageQueueId_t PotentiometerHandle;

void StartTask_LVGLExample(void *argument)
{
  lv_init();
  lv_tick_set_cb(HAL_GetTick);
  lcd_init(); 
  touch_init();
  // Setup Display
  lv_display_t * disp = lv_display_create(LCD_W, LCD_H);
  static uint8_t draw_buf[LCD_W * (LCD_H / 10) * 2];
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

  // Create the button on the active screen
  lv_obj_t * btn = lv_button_create(screen);
  lv_obj_set_size(btn, 160, 60);
  lv_obj_center(btn); // Put it right in the middle

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

    // 3. Blast the pixels to the FSMC RAM
    // Since FSMC is mapped directly to memory, we can write directly to the LCD_RAM register
    for(uint32_t i = 0; i < total_pixels; i++) {
        LCD->LCD_RAM = color_p[i]; 
    }

    // 4. IMPORTANT: Tell LVGL the transfer is complete
    lv_display_flush_ready(disp);
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