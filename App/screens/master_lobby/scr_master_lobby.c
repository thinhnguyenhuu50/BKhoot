#include "scr_master_lobby.h"
#include "ui_styles.h"
#include "game_logic.h"
#include "gui_app.h"
#include <stddef.h>

static lv_obj_t * scr_master_lobby;
static lv_obj_t * label_lobby_count = NULL;

static void master_start_quiz_cb(lv_event_t * e) {
    game_master_start_quiz();
}

void gui_load_master_lobby_screen(void) {
    scr_master_lobby = lv_obj_create(NULL);
    lv_obj_add_style(scr_master_lobby, &style_screen, 0);

    lv_obj_t * header = lv_obj_create(scr_master_lobby);
    lv_obj_set_size(header, LV_PCT(100), 60);
    lv_obj_add_style(header, &style_header_panel, 0);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
    
    lv_obj_t * title = lv_label_create(header);
    lv_label_set_text(title, "Lobby");
    lv_obj_add_style(title, &style_title, 0);
    lv_obj_center(title);

    create_home_btn(scr_master_lobby, LV_ALIGN_TOP_LEFT, 10, 10);

    label_lobby_count = lv_label_create(scr_master_lobby);
    lv_label_set_text(label_lobby_count, "Players: 0");
    lv_obj_add_style(label_lobby_count, &style_title, 0);
    lv_obj_align(label_lobby_count, LV_ALIGN_CENTER, 0, -20);

    lv_obj_t * btn_start = lv_btn_create(scr_master_lobby);
    lv_obj_add_style(btn_start, &style_btn_primary, 0);
    lv_obj_set_size(btn_start, 180, 50);
    lv_obj_align(btn_start, LV_ALIGN_CENTER, 0, 60);
    lv_obj_t * lbl_start = lv_label_create(btn_start);
    lv_label_set_text(lbl_start, "Start Quiz");
    lv_obj_center(lbl_start);
    lv_obj_add_event_cb(btn_start, master_start_quiz_cb, LV_EVENT_CLICKED, NULL);

    lv_scr_load_anim(scr_master_lobby, LV_SCR_LOAD_ANIM_FADE_ON, 300, 0, true);
}

void gui_update_master_lobby_count(int count) {
    if(label_lobby_count != NULL) {
        lv_label_set_text_fmt(label_lobby_count, "Players: %d", count);
    }
}
