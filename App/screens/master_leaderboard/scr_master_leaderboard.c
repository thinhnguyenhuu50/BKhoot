#include "scr_master_leaderboard.h"
#include "ui_styles.h"
#include "game_logic.h"
#include <stddef.h>

static lv_obj_t * scr_master_leaderboard;

static void master_next_cb(lv_event_t * e) {
    game_master_next_question();
}

void gui_load_master_leaderboard_screen(void) {
    scr_master_leaderboard = lv_obj_create(NULL);
    lv_obj_add_style(scr_master_leaderboard, &style_screen, 0);

    lv_obj_t * title = lv_label_create(scr_master_leaderboard);
    lv_label_set_text(title, "Results");
    lv_obj_add_style(title, &style_title, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

    create_home_btn(scr_master_leaderboard, LV_ALIGN_TOP_LEFT, 10, 10);

    lv_obj_t * btn_next = lv_btn_create(scr_master_leaderboard);
    lv_obj_add_style(btn_next, &style_btn_primary, 0);
    lv_obj_set_size(btn_next, 180, 50);
    lv_obj_align(btn_next, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_t * lbl_next = lv_label_create(btn_next);
    lv_label_set_text(lbl_next, "Next");
    lv_obj_center(lbl_next);
    
    // Trigger next question
    lv_obj_add_event_cb(btn_next, master_next_cb, LV_EVENT_CLICKED, NULL);

    lv_scr_load_anim(scr_master_leaderboard, LV_SCR_LOAD_ANIM_FADE_ON, 300, 0, true);
}
