#include "scr_slave_feedback.h"
#include "ui_styles.h"
#include <stddef.h>

static lv_obj_t * scr_slave_feedback;
static lv_obj_t * label_feedback_msg = NULL;
static lv_obj_t * label_score = NULL;

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

    lv_scr_load_anim(scr_slave_feedback, LV_SCR_LOAD_ANIM_FADE_ON, 300, 0, true);
}
