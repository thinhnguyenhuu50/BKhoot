#include "scr_slave_answer.h"
#include "ui_styles.h"
#include "game_logic.h"
#include "rs232.h"
#include <stddef.h>
#include <stdint.h>

static lv_obj_t * scr_slave_answer;

static void slave_answer_btn_cb(lv_event_t * e) {
    int answer_idx = (int)(intptr_t)lv_event_get_user_data(e);
    const char *colors[] = {"Red", "Blue", "Yellow", "Green"};
    if (answer_idx >= 0 && answer_idx < 4) {
        debug_log("Color chosen: %s\r\n", colors[answer_idx]);
    }
    game_slave_submit_answer(answer_idx);
}

void gui_load_slave_answer_screen(void) {
    scr_slave_answer = lv_obj_create(NULL);
    lv_obj_add_style(scr_slave_answer, &style_screen, 0);

    create_home_btn(scr_slave_answer, LV_ALIGN_TOP_LEFT, 10, 10);

    int padding = 15;
    int btn_w = (240 - padding * 3) / 2;
    int btn_h = (320 - padding * 3) / 2;

    for(int i = 0; i < 4; i++) {
        lv_obj_t * btn = lv_btn_create(scr_slave_answer);
        lv_obj_add_style(btn, &style_btn_ans[i], 0);
        lv_obj_set_size(btn, btn_w, btn_h);
        
        int x_ofs = (i % 2 == 0) ? -(btn_w/2 + padding/2) : (btn_w/2 + padding/2);
        int y_ofs = (i < 2) ? -(btn_h/2 + padding/2) : (btn_h/2 + padding/2);
        lv_obj_align(btn, LV_ALIGN_CENTER, x_ofs, y_ofs);

        lv_obj_add_event_cb(btn, slave_answer_btn_cb, LV_EVENT_CLICKED, (void*)(intptr_t)i);
    }

    lv_scr_load(scr_slave_answer);
}
