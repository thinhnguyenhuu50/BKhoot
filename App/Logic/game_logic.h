#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <stdint.h>

#define DEFAULT_QUESTION_TIMER_MS 30000
#define MAX_PLAYERS 20

// One row of the master's ranking, shown to the host as "P<player_id>"
typedef struct {
    uint8_t player_id;
    uint8_t score;
} PlayerRank_t;

typedef enum {
    GAME_ROLE_NONE,
    GAME_ROLE_MASTER,
    GAME_ROLE_SLAVE
} GameRole_t;

typedef enum {
    GAME_STATE_INIT,
    GAME_STATE_LOBBY,
    GAME_STATE_QUESTION,
    GAME_STATE_RESULTS,
    GAME_STATE_FEEDBACK
} GameState_t;

// Initialize the game state
void game_init(void);

// Set the current board role
void game_set_role(GameRole_t role);
GameRole_t game_get_role(void);

// Task function to be called from FreeRTOS
void game_task_func(void *argument);

// --- Callbacks from UI ---

// Master starts the quiz
void game_master_start_quiz(void);

// Master goes to next question
void game_master_next_question(void);

// Slave initiates scan for masters
void game_slave_scan_hosts(void);

// Slave joins a specific host
void game_slave_join_host(int host_id);

// Slave submits an answer (0: Red, 1: Blue, 2: Yellow, 3: Green)
void game_slave_submit_answer(int answer_idx);

// Master timer finishes (called from the LVGL task, the game task then ends the question)
void game_master_timer_timeout(void);

// Send a command to the ESP8266 via UART2
void comm_send_cmd(uint8_t cmd, uint8_t *payload, uint8_t len);

#endif // GAME_LOGIC_H
