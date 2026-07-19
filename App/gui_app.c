#include "gui_app.h"
#include "game_logic.h"
#include <stdio.h>
#include "rs232.h"

// Screen objects
static lv_obj_t * scr_role_selection;
static lv_obj_t * scr_master_lobby;
static lv_obj_t * scr_master_question;
static lv_obj_t * scr_master_leaderboard;
static lv_obj_t * scr_slave_scan;
static lv_obj_t * scr_slave_waiting;
static lv_obj_t * scr_slave_answer;
static lv_obj_t * scr_slave_feedback;

// Styles
static lv_style_t style_screen;
static lv_style_t style_btn_primary;
static lv_style_t style_btn_ans[4]; // Red, Blue, Yellow, Green
static lv_style_t style_title;
static lv_style_t style_text;
static lv_style_t style_header_panel;

// Global UI elements that need updating
static lv_obj_t * label_lobby_count = NULL;
static lv_obj_t * list_hosts = NULL;
static lv_obj_t * bar_timer = NULL;
static lv_obj_t * label_question = NULL;
static lv_obj_t * label_feedback_msg = NULL;
static lv_obj_t * label_score = NULL;

static void init_styles(void) {
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

// --- Callbacks ---

static void home_btn_cb(lv_event_t * e) {
    gui_load_role_selection_screen();
}

static void create_home_btn(lv_obj_t * parent, lv_align_t align, int32_t x_ofs, int32_t y_ofs) {
    lv_obj_t * btn = lv_btn_create(parent);
    lv_obj_set_size(btn, 40, 40);
    lv_obj_add_style(btn, &style_btn_primary, 0);
    lv_obj_align(btn, align, x_ofs, y_ofs);
    lv_obj_t * lbl = lv_label_create(btn);
    lv_label_set_text(lbl, LV_SYMBOL_HOME);
    lv_obj_center(lbl);
    lv_obj_add_event_cb(btn, home_btn_cb, LV_EVENT_CLICKED, NULL);
}

static void role_btn_event_cb(lv_event_t * e) {
    int role = (int)lv_event_get_user_data(e);
    if(role == 0) {
        game_set_role(GAME_ROLE_MASTER);
        gui_load_master_lobby_screen();
    } else {
        game_set_role(GAME_ROLE_SLAVE);
        gui_load_slave_scan_screen();
    }
}

static void master_start_quiz_cb(lv_event_t * e) {
    game_master_start_quiz();
}

static void slave_scan_refresh_cb(lv_event_t * e) {
    game_slave_scan_hosts();
    gui_slave_clear_host_list();
}

static void host_list_btn_cb(lv_event_t * e) {
    int host_id = (int)lv_event_get_user_data(e);
    game_slave_join_host(host_id);
}

static void slave_answer_btn_cb(lv_event_t * e) {
    int answer_idx = (int)lv_event_get_user_data(e);
    const char *colors[] = {"Red", "Blue", "Yellow", "Green"};
    if (answer_idx >= 0 && answer_idx < 4) {
        debug_log("Color chosen: %s\r\n", colors[answer_idx]);
    }
    game_slave_submit_answer(answer_idx);
}

static void master_answer_btn_cb(lv_event_t * e) {
    int answer_idx = (int)lv_event_get_user_data(e);
    const char *colors[] = {"Red", "Blue", "Yellow", "Green"};
    if (answer_idx >= 0 && answer_idx < 4) {
        debug_log("Master chosen: %s\r\n", colors[answer_idx]);
    }
    // Master doesn't submit answer to itself
}

// --- Screen Implementations ---

void gui_app_init(void) {
    init_styles();
    gui_load_role_selection_screen();
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

    lv_scr_load(scr_master_lobby);
}

void gui_update_master_lobby_count(int count) {
    if(label_lobby_count != NULL) {
        lv_label_set_text_fmt(label_lobby_count, "Players: %d", count);
    }
}

void gui_load_master_question_screen(const char* question) {
    scr_master_question = lv_obj_create(NULL);
    lv_obj_add_style(scr_master_question, &style_screen, 0);

    // Header
    lv_obj_t * header = lv_obj_create(scr_master_question);
    lv_obj_set_size(header, LV_PCT(100), 100);
    lv_obj_add_style(header, &style_header_panel, 0);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);

    label_question = lv_label_create(header);
    lv_label_set_text(label_question, question);
    lv_obj_set_width(label_question, LV_PCT(70));
    lv_label_set_long_mode(label_question, LV_LABEL_LONG_WRAP);
    lv_obj_add_style(label_question, &style_text, 0);
    lv_obj_align(label_question, LV_ALIGN_CENTER, 20, 0);

    create_home_btn(scr_master_question, LV_ALIGN_TOP_LEFT, 10, 10);

    // Timer Bar
    bar_timer = lv_bar_create(scr_master_question);
    lv_obj_set_size(bar_timer, LV_PCT(90), 15);
    lv_obj_align(bar_timer, LV_ALIGN_TOP_MID, 0, 110);
    lv_bar_set_value(bar_timer, 100, LV_ANIM_OFF);

    // Answer grid
    int padding = 20;
    int btn_w = (240 - padding * 3) / 2; // (Screen_W - 3*pad) / 2
    int btn_h = 70;

    for(int i = 0; i < 4; i++) {
        lv_obj_t * btn = lv_btn_create(scr_master_question);
        lv_obj_add_style(btn, &style_btn_ans[i], 0);
        lv_obj_set_size(btn, btn_w, btn_h);
        
        int x_ofs = (i % 2 == 0) ? -(btn_w/2 + padding/2) : (btn_w/2 + padding/2);
        int y_ofs = (i < 2) ? 140 : (140 + btn_h + padding);
        lv_obj_align(btn, LV_ALIGN_TOP_MID, x_ofs, y_ofs);
        
        lv_obj_add_event_cb(btn, master_answer_btn_cb, LV_EVENT_CLICKED, (void*)i);
    }

    lv_scr_load(scr_master_question);
}

void gui_update_master_timer(int percent) {
    if(bar_timer != NULL) {
        lv_bar_set_value(bar_timer, percent, LV_ANIM_ON);
    }
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
    
    // In a real app, you'd trigger next question
    // lv_obj_add_event_cb(btn_next, master_next_cb, LV_EVENT_CLICKED, NULL);

    lv_scr_load(scr_master_leaderboard);
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
        lv_obj_add_event_cb(btn, host_list_btn_cb, LV_EVENT_CLICKED, (void*)host_id);
    }
}

void gui_slave_clear_host_list(void) {
    if(list_hosts != NULL) {
        lv_obj_clean(list_hosts);
    }
}

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

    lv_scr_load(scr_slave_waiting);
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

        lv_obj_add_event_cb(btn, slave_answer_btn_cb, LV_EVENT_CLICKED, (void*)i);
    }

    lv_scr_load(scr_slave_answer);
}

void gui_load_slave_feedback_screen(bool correct, int current_score) {
    scr_slave_feedback = lv_obj_create(NULL);
    
    // Background color based on correct/incorrect
    if(correct) {
        lv_obj_set_style_bg_color(scr_slave_feedback, lv_color_hex(0x26890C), 0);
    } else {
        lv_obj_set_style_bg_color(scr_slave_feedback, lv_color_hex(0xE21B3C), 0);
    }

    label_feedback_msg = lv_label_create(scr_slave_feedback);
    lv_label_set_text(label_feedback_msg, correct ? "Correct!" : "Incorrect!");
    lv_obj_set_style_text_font(label_feedback_msg, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(label_feedback_msg, lv_color_white(), 0);
    lv_obj_align(label_feedback_msg, LV_ALIGN_CENTER, 0, -20);

    label_score = lv_label_create(scr_slave_feedback);
    lv_label_set_text_fmt(label_score, "Score: %d", current_score);
    lv_obj_set_style_text_font(label_score, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(label_score, lv_color_white(), 0);
    lv_obj_align(label_score, LV_ALIGN_CENTER, 0, 20);

    create_home_btn(scr_slave_feedback, LV_ALIGN_TOP_LEFT, 10, 10);

    lv_scr_load(scr_slave_feedback);
}

#include "cmsis_os.h"
#include "lcd.h"
#include "touch.h"
#include "dma.h"

static lv_display_t * current_disp;

void HAL_DMA_TxCpltCallback(DMA_HandleTypeDef *hdma)
{
    if (hdma->Instance == DMA2_Stream0) {
        if (current_disp != NULL) {
            lv_display_flush_ready(current_disp);
        }
    }
}

static void my_disp_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    uint16_t * color_p = (uint16_t *)px_map;
    lcd_set_address(area->x1, area->y1, area->x2, area->y2);
    uint32_t width = lv_area_get_width(area);
    uint32_t height = lv_area_get_height(area);
    uint32_t total_pixels = width * height;
    current_disp = disp;
    HAL_DMA_Start_IT(&hdma_memtomem_dma2_stream0, (uint32_t)color_p, (uint32_t)&LCD->LCD_RAM, total_pixels);
}

static void my_touchpad_read(lv_indev_t * indev, lv_indev_data_t * data) {
    if(touch_IsTouched()) {
        touch_Scan();
        data->state = LV_INDEV_STATE_PRESSED;
        data->point.x = touch_GetX();
        data->point.y = touch_GetY();
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

void StartTask_LVGL(void *argument) {
    lv_init();
    lv_tick_set_cb(HAL_GetTick);
    lcd_init(); 
    touch_Init();
    
    // Setup Display
    lv_display_t * disp = lv_display_create(LCD_W, LCD_H);
    uint32_t buf_size = LCD_W * (LCD_H / 10) * 2;
    uint8_t * draw_buf = (uint8_t *)pvPortMalloc(buf_size);
    if (draw_buf == NULL) {
        debug_log("Failed to allocate draw_buf!\r\n");
        vTaskDelete(NULL);
    }
    lv_display_set_buffers(disp, draw_buf, NULL, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(disp, my_disp_flush);

    // Register Touchpad
    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, my_touchpad_read);

    gui_app_init();
    debug_log("GUI initialized\r\n");
    for(;;) {
        lv_timer_handler();
        osDelay(5);
    }
}
