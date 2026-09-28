#include "scr_slave_timeup.h"
#include "ui_styles.h"
#include <stddef.h>

static lv_obj_t * scr_slave_timeup;

void gui_load_slave_timeup_screen(void) {
    if (scr_slave_timeup == NULL) {
        scr_slave_timeup = lv_obj_create(NULL);
        lv_obj_set_style_bg_color(scr_slave_timeup, lv_color_hex(0x003366), 0);

        lv_obj_t * label_msg = lv_label_create(scr_slave_timeup);
        lv_label_set_text(label_msg, "Time's up!");
        lv_obj_set_style_text_font(label_msg, &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(label_msg, lv_color_white(), 0);
        lv_obj_align(label_msg, LV_ALIGN_CENTER, 0, -20);

        lv_obj_t * label_hint = lv_label_create(scr_slave_timeup);
        lv_label_set_text(label_hint, "No answer counted");
        lv_obj_set_style_text_font(label_hint, &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(label_hint, lv_color_white(), 0);
        lv_obj_align(label_hint, LV_ALIGN_CENTER, 0, 20);

        create_home_btn(scr_slave_timeup, LV_ALIGN_TOP_LEFT, 10, 10);
    }

    lv_scr_load_anim(scr_slave_timeup, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
}
