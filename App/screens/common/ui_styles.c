#include "ui_styles.h"
#include "gui_app.h" // For gui_load_role_selection_screen() since home_btn_cb calls it

lv_style_t style_screen;
lv_style_t style_btn_primary;
lv_style_t style_btn_ans[4];
lv_style_t style_title;
lv_style_t style_text;
lv_style_t style_header_panel;

void init_styles(void) {
    // Screen background (Light Blue/White)
    lv_style_init(&style_screen);
    lv_style_set_bg_color(&style_screen, lv_color_hex(0xF0F8FF)); 
    lv_style_set_text_color(&style_screen, lv_color_hex(0x333333));

    // Primary Button (Deep Blue)
    lv_style_init(&style_btn_primary);
    lv_style_set_bg_color(&style_btn_primary, lv_color_hex(0x007BFF));
    lv_style_set_text_color(&style_btn_primary, lv_color_white());
    lv_style_set_radius(&style_btn_primary, 10);
    lv_style_set_shadow_width(&style_btn_primary, 5);
    lv_style_set_shadow_color(&style_btn_primary, lv_color_hex(0xCCCCCC));
    lv_style_set_shadow_ofs_y(&style_btn_primary, 3);

    // Answer Buttons (Red, Blue, Yellow, Green)
    lv_color_t ans_colors[4] = {
        lv_color_hex(0xE21B3C), // Red
        lv_color_hex(0x1368CE), // Blue
        lv_color_hex(0xD89E00), // Yellow
        lv_color_hex(0x26890C)  // Green
    };

    for(int i = 0; i < 4; i++) {
        lv_style_init(&style_btn_ans[i]);
        lv_style_set_bg_color(&style_btn_ans[i], ans_colors[i]);
        lv_style_set_radius(&style_btn_ans[i], 8);
        lv_style_set_shadow_width(&style_btn_ans[i], 5);
        lv_style_set_shadow_color(&style_btn_ans[i], lv_color_hex(0x999999));
        lv_style_set_shadow_ofs_y(&style_btn_ans[i], 4);
    }

    // Texts
    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, &lv_font_montserrat_14);
    lv_style_set_text_color(&style_title, lv_color_hex(0x003366));

    lv_style_init(&style_text);
    lv_style_set_text_font(&style_text, &lv_font_montserrat_14);

    // Header Panel
    lv_style_init(&style_header_panel);
    lv_style_set_bg_color(&style_header_panel, lv_color_white());
    lv_style_set_radius(&style_header_panel, 0);
    lv_style_set_border_width(&style_header_panel, 0);
    lv_style_set_shadow_width(&style_header_panel, 10);
    lv_style_set_shadow_color(&style_header_panel, lv_color_hex(0xDDDDDD));
}

void home_btn_cb(lv_event_t * e) {
    gui_load_role_selection_screen();
}

void create_home_btn(lv_obj_t * parent, lv_align_t align, int32_t x_ofs, int32_t y_ofs) {
    lv_obj_t * btn = lv_btn_create(parent);
    lv_obj_set_size(btn, 40, 40);
    lv_obj_add_style(btn, &style_btn_primary, 0);
    lv_obj_align(btn, align, x_ofs, y_ofs);
    lv_obj_t * lbl = lv_label_create(btn);
    lv_label_set_text(lbl, LV_SYMBOL_HOME);
    lv_obj_center(lbl);
    lv_obj_add_event_cb(btn, home_btn_cb, LV_EVENT_CLICKED, NULL);
}
