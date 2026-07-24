#include "scr_slave_waiting.h"
#include "ui_styles.h"
#include <stddef.h>

static lv_obj_t * scr_slave_waiting;

void gui_load_slave_waiting_screen(void) {
    scr_slave_waiting = lv_obj_create(NULL);
    lv_obj_add_style(scr_slave_waiting, &style_screen, 0);

    lv_obj_t * spinner = lv_spinner_create(scr_slave_waiting);
    lv_obj_set_size(spinner, 80, 80);
    lv_obj_align(spinner, LV_ALIGN_CENTER, 0, -30);

    lv_obj_t * label = lv_label_create(scr_slave_waiting);
    lv_label_set_text(label, "Waiting for Host...");
    lv_obj_add_style(label, &style_text, 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 40);

    create_home_btn(scr_slave_waiting, LV_ALIGN_TOP_LEFT, 10, 10);

    lv_scr_load_anim(scr_slave_waiting, LV_SCR_LOAD_ANIM_FADE_ON, 300, 0, true);
}
