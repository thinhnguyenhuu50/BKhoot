#include "scr_slave_scan.h"
#include "ui_styles.h"
#include "game_logic.h"
#include <stddef.h>
#include <stdint.h>

static lv_obj_t * scr_slave_scan;
static lv_obj_t * list_hosts = NULL;

static void slave_scan_refresh_cb(lv_event_t * e) {
    game_slave_scan_hosts();
    gui_slave_clear_host_list();
}

static void host_list_btn_cb(lv_event_t * e) {
    int host_id = (int)(intptr_t)lv_event_get_user_data(e);
    game_slave_join_host(host_id);
}

void gui_load_slave_scan_screen(void) {
    scr_slave_scan = lv_obj_create(NULL);
    lv_obj_add_style(scr_slave_scan, &style_screen, 0);

    lv_obj_t * title = lv_label_create(scr_slave_scan);
    lv_label_set_text(title, "Available Hosts");
    lv_obj_add_style(title, &style_title, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

    create_home_btn(scr_slave_scan, LV_ALIGN_TOP_LEFT, 10, 10);

    lv_obj_t * btn_scan = lv_btn_create(scr_slave_scan);
    lv_obj_add_style(btn_scan, &style_btn_primary, 0);
    lv_obj_set_size(btn_scan, 120, 40);
    lv_obj_align(btn_scan, LV_ALIGN_TOP_RIGHT, -10, 40);
    lv_obj_t * lbl_scan = lv_label_create(btn_scan);
    lv_label_set_text(lbl_scan, "Refresh");
    lv_obj_center(lbl_scan);
    lv_obj_add_event_cb(btn_scan, slave_scan_refresh_cb, LV_EVENT_CLICKED, NULL);

    list_hosts = lv_list_create(scr_slave_scan);
    lv_obj_set_size(list_hosts, LV_PCT(90), LV_PCT(60));
    lv_obj_align(list_hosts, LV_ALIGN_CENTER, 0, 20);

    lv_scr_load(scr_slave_scan);
}

void gui_slave_add_host_to_list(const char* host_name, int host_id) {
    if(list_hosts != NULL) {
        lv_obj_t * btn = lv_list_add_btn(list_hosts, LV_SYMBOL_WIFI, host_name);
        lv_obj_add_event_cb(btn, host_list_btn_cb, LV_EVENT_CLICKED, (void*)(intptr_t)host_id);
    }
}

void gui_slave_clear_host_list(void) {
    if(list_hosts != NULL) {
        lv_obj_clean(list_hosts);
    }
}
