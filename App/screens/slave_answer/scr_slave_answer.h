#ifndef SCR_SLAVE_ANSWER_H
#define SCR_SLAVE_ANSWER_H

#include <stdint.h>

// player_id: ID assigned by the master (shown as "P<id>"), 0 if not assigned yet
void gui_load_slave_answer_screen(const char* question, const char *options[4], uint32_t duration_ms, uint8_t player_id);

#endif // SCR_SLAVE_ANSWER_H
