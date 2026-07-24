#include "scr_slave_answer.h"
#include "ui_styles.h"
#include "game_logic.h"
#include "rs232.h"
#include <stddef.h>
#include <stdint.h>

static lv_obj_t * scr_slave_answer;
static lv_obj_t * label_question = NULL;
static lv_obj_t * bar_timer = NULL;

static void set_timer_val(void * bar, int32_t val) {
    lv_bar_set_value((lv_obj_t *)bar, val, LV_ANIM_ON);
}

static void slave_answer_btn_cb(lv_event_t * e) {
    int answer_idx = (int)(intptr_t)lv_event_get_user_data(e);
    const char *colors[] = {"Red", "Blue", "Yellow", "Green"};
    if (answer_idx >= 0 && answer_idx < 4) {
        debug_log("Color chosen: %s\r\n", colors[answer_idx]);
    }
    
    if(bar_timer != NULL) {
        lv_anim_del(bar_timer, set_timer_val);
    }
    
    game_slave_submit_answer(answer_idx);
}

static void scr_delete_cb(lv_event_t * e) {
    if(bar_timer != NULL) {
        lv_anim_del(bar_timer, set_timer_val);
        bar_timer = NULL;
    }
}

void gui_load_slave_answer_screen(const char* question, uint32_t duration_ms) {
    scr_slave_answer = lv_obj_create(NULL);
    lv_obj_add_style(scr_slave_answer, &style_screen, 0);
    lv_obj_add_event_cb(scr_slave_answer, scr_delete_cb, LV_EVENT_DELETE, NULL);

    // Header
    lv_obj_t * header = lv_obj_create(scr_slave_answer);
    lv_obj_set_size(header, LV_PCT(100), 100);
    lv_obj_add_style(header, &style_header_panel, 0);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);

    label_question = lv_label_create(header);
    lv_label_set_text(label_question, question);
    lv_obj_set_width(label_question, LV_PCT(70));
    lv_label_set_long_mode(label_question, LV_LABEL_LONG_WRAP);
    lv_obj_add_style(label_question, &style_text, 0);
    lv_obj_align(label_question, LV_ALIGN_CENTER, 20, 0);

    create_home_btn(scr_slave_answer, LV_ALIGN_TOP_LEFT, 10, 10);

    // Timer Bar
    bar_timer = lv_bar_create(scr_slave_answer);
    lv_obj_set_size(bar_timer, LV_PCT(90), 15);
    lv_obj_align(bar_timer, LV_ALIGN_TOP_MID, 0, 110);
    lv_bar_set_range(bar_timer, 0, 100);
    lv_bar_set_value(bar_timer, 100, LV_ANIM_OFF);

    // Answer grid
    int padding = 20;
    int btn_w = (240 - padding * 3) / 2; // (Screen_W - 3*pad) / 2
    int btn_h = 70;

    for(int i = 0; i < 4; i++) {
        lv_obj_t * btn = lv_btn_create(scr_slave_answer);
        lv_obj_add_style(btn, &style_btn_ans[i], 0);
        lv_obj_set_size(btn, btn_w, btn_h);
        
        int x_ofs = (i % 2 == 0) ? -(btn_w/2 + padding/2) : (btn_w/2 + padding/2);
        int y_ofs = (i < 2) ? 140 : (140 + btn_h + padding);
        lv_obj_align(btn, LV_ALIGN_TOP_MID, x_ofs, y_ofs);
        
        lv_obj_add_event_cb(btn, slave_answer_btn_cb, LV_EVENT_CLICKED, (void*)(intptr_t)i);
    }

    // Timer Animation
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, set_timer_val);
    lv_anim_set_duration(&a, duration_ms);
    lv_anim_set_var(&a, bar_timer);
    lv_anim_set_values(&a, 100, 0);
    lv_anim_start(&a);

    lv_scr_load(scr_slave_answer);
}
