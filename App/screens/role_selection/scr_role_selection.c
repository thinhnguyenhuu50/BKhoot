#include "scr_role_selection.h"
#include "ui_styles.h"
#include "game_logic.h"
#include "gui_app.h" 
#include <stdint.h>

static lv_obj_t * scr_role_selection;

static void role_btn_event_cb(lv_event_t * e) {
    int role = (int)(intptr_t)lv_event_get_user_data(e);
    if(role == 0) {
        game_set_role(GAME_ROLE_MASTER);
        gui_load_master_lobby_screen();
    } else {
        game_set_role(GAME_ROLE_SLAVE);
        gui_load_slave_scan_screen();
    }
}

void gui_load_role_selection_screen(void) {
    scr_role_selection = lv_obj_create(NULL);
    lv_obj_add_style(scr_role_selection, &style_screen, 0);

    lv_obj_t * title = lv_label_create(scr_role_selection);
    lv_label_set_text(title, "BKhoot");
    lv_obj_add_style(title, &style_title, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 40);

    lv_obj_t * subtitle = lv_label_create(scr_role_selection);
    lv_label_set_text(subtitle, "Select your role");
    lv_obj_add_style(subtitle, &style_text, 0);
    lv_obj_align_to(subtitle, title, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    lv_obj_t * btn_master = lv_btn_create(scr_role_selection);
    lv_obj_add_style(btn_master, &style_btn_primary, 0);
    lv_obj_set_size(btn_master, 180, 50);
    lv_obj_align(btn_master, LV_ALIGN_CENTER, 0, -20);
    lv_obj_t * lbl_master = lv_label_create(btn_master);
    lv_label_set_text(lbl_master, "Host (Master)");
    lv_obj_center(lbl_master);
    lv_obj_add_event_cb(btn_master, role_btn_event_cb, LV_EVENT_CLICKED, (void*)0);

    lv_obj_t * btn_slave = lv_btn_create(scr_role_selection);
    lv_obj_add_style(btn_slave, &style_btn_primary, 0);
    lv_obj_set_size(btn_slave, 180, 50);
    lv_obj_align(btn_slave, LV_ALIGN_CENTER, 0, 50);
    lv_obj_t * lbl_slave = lv_label_create(btn_slave);
    lv_label_set_text(lbl_slave, "Join (Slave)");
    lv_obj_center(lbl_slave);
    lv_obj_add_event_cb(btn_slave, role_btn_event_cb, LV_EVENT_CLICKED, (void*)1);

    lv_scr_load(scr_role_selection);
}
