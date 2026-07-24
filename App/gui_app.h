#ifndef GUI_APP_H
#define GUI_APP_H

#include "lvgl.h"
#include <stdbool.h>

// Include all screen headers so other files can just include gui_app.h if they want
#include "scr_role_selection.h"
#include "scr_master_lobby.h"
#include "scr_master_question.h"
#include "scr_master_leaderboard.h"
#include "scr_slave_scan.h"
#include "scr_slave_waiting.h"
#include "scr_slave_answer.h"
#include "scr_slave_feedback.h"

#include "cmsis_os.h"

extern osMutexId_t lvgl_mutexHandle;

static inline void app_lv_lock(void) {
    if (lvgl_mutexHandle != NULL) {
        osMutexAcquire(lvgl_mutexHandle, osWaitForever);
    }
}

static inline void app_lv_unlock(void) {
    if (lvgl_mutexHandle != NULL) {
        osMutexRelease(lvgl_mutexHandle);
    }
}

// Initialize the GUI and load the initial role selection screen
void gui_app_init(void);

#endif // GUI_APP_H
