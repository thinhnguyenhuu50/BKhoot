#ifndef UI_STYLES_H
#define UI_STYLES_H

#include "lvgl.h"

// Expose styles
extern lv_style_t style_screen;
extern lv_style_t style_btn_primary;
extern lv_style_t style_btn_ans[4]; // Red, Blue, Yellow, Green
extern lv_style_t style_title;
extern lv_style_t style_text;
extern lv_style_t style_header_panel;

// Initialize styles
void init_styles(void);

// Common UI elements
void create_home_btn(lv_obj_t * parent, lv_align_t align, int32_t x_ofs, int32_t y_ofs);
void home_btn_cb(lv_event_t * e);

#endif // UI_STYLES_H
