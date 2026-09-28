#include "scr_master_congrats.h"
#include "ui_styles.h"
#include <stddef.h>

static lv_obj_t * scr_master_congrats;
static lv_obj_t * list_ranking = NULL;

void gui_load_master_congrats_screen(const PlayerRank_t *ranking, int count) {
    if (scr_master_congrats == NULL) {
        scr_master_congrats = lv_obj_create(NULL);
        lv_obj_add_style(scr_master_congrats, &style_screen, 0);

        lv_obj_t * title = lv_label_create(scr_master_congrats);
        lv_label_set_text(title, "Congratulations!");
        lv_obj_add_style(title, &style_title, 0);
        lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

        lv_obj_t * subtitle = lv_label_create(scr_master_congrats);
        lv_label_set_text(subtitle, "Final Ranking");
        lv_obj_add_style(subtitle, &style_text, 0);
        lv_obj_align(subtitle, LV_ALIGN_TOP_MID, 0, 42);

        // The quiz is over, so home is the only way out
        create_home_btn(scr_master_congrats, LV_ALIGN_TOP_LEFT, 10, 10);

        list_ranking = create_ranking_list(scr_master_congrats);
        lv_obj_set_size(list_ranking, LV_PCT(90), 230);
        lv_obj_align(list_ranking, LV_ALIGN_TOP_MID, 0, 70);
    }

    update_ranking_list(list_ranking, ranking, count);

    lv_scr_load_anim(scr_master_congrats, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
}
