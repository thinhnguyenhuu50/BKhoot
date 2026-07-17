#ifndef GUI_APP_H
#define GUI_APP_H

#include "lvgl.h"
#include <stdbool.h>

// Initialize the GUI and load the initial role selection screen
void gui_app_init(void);

// --- Initial Screens ---
void gui_load_role_selection_screen(void);

// --- Master Screens ---
void gui_load_master_lobby_screen(void);
void gui_update_master_lobby_count(int count);

void gui_load_master_question_screen(const char* question);
void gui_update_master_timer(int percent);

void gui_load_master_leaderboard_screen(void);

// --- Slave Screens ---
void gui_load_slave_scan_screen(void);
void gui_slave_add_host_to_list(const char* host_name, int host_id);
void gui_slave_clear_host_list(void);

void gui_load_slave_waiting_screen(void);

void gui_load_slave_answer_screen(void);

void gui_load_slave_feedback_screen(bool correct, int current_score);

#endif // GUI_APP_H
