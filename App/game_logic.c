#include "game_logic.h"
#include "gui_app.h"
#include "cmsis_os.h" // For FreeRTOS functions like osDelay
#include <stdio.h>
#include <string.h>

// Game state variables
static GameRole_t current_role = GAME_ROLE_NONE;
static GameState_t current_state = GAME_STATE_INIT;

// Master state
static int connected_players = 0;
static int current_question_timer = 0;

// Slave state
static int current_score = 0;

void game_init(void) {
    current_role = GAME_ROLE_NONE;
    current_state = GAME_STATE_INIT;
}

void game_set_role(GameRole_t role) {
    current_role = role;
    if(role == GAME_ROLE_MASTER) {
        current_state = GAME_STATE_LOBBY;
        connected_players = 0;
    } else if(role == GAME_ROLE_SLAVE) {
        current_state = GAME_STATE_INIT; // Scanning state
    }
}

GameRole_t game_get_role(void) {
    return current_role;
}

// Master Callbacks
void game_master_start_quiz(void) {
    if(current_role == GAME_ROLE_MASTER && current_state == GAME_STATE_LOBBY) {
        current_state = GAME_STATE_QUESTION;
        // In a real app, send ESP-NOW broadcast to start quiz
        gui_load_master_question_screen("What is the capital of France?");
        current_question_timer = 100; // 100%
        gui_update_master_timer(current_question_timer);
    }
}

void game_master_next_question(void) {
    if(current_role == GAME_ROLE_MASTER) {
        current_state = GAME_STATE_QUESTION;
        gui_load_master_question_screen("Next question...");
    }
}

// Slave Callbacks
void game_slave_scan_hosts(void) {
    if(current_role == GAME_ROLE_SLAVE) {
        // Mocking finding hosts
        gui_slave_add_host_to_list("Host_1 (Physics)", 1);
        gui_slave_add_host_to_list("Host_2 (Math)", 2);
    }
}

void game_slave_join_host(int host_id) {
    if(current_role == GAME_ROLE_SLAVE) {
        current_state = GAME_STATE_LOBBY;
        // Send join request via ESP-NOW
        gui_load_slave_waiting_screen();
    }
}

void game_slave_submit_answer(int answer_idx) {
    if(current_role == GAME_ROLE_SLAVE && current_state == GAME_STATE_QUESTION) {
        current_state = GAME_STATE_FEEDBACK;
        // Send answer to master via ESP-NOW
        // For now, mock feedback directly
        bool correct = (answer_idx == 1); // Mock correct answer
        if(correct) current_score += 100;
        gui_load_slave_feedback_screen(correct, current_score);
    }
}

// Main task loop
void game_task_func(void *argument) {
    while(1) {
        // Handle state machine logic that requires periodic update
        if(current_role == GAME_ROLE_MASTER) {
            if(current_state == GAME_STATE_LOBBY) {
                // Mock player joining
                // gui_update_master_lobby_count(connected_players);
            } else if(current_state == GAME_STATE_QUESTION) {
                // Update timer
                if(current_question_timer > 0) {
                    current_question_timer -= 2; // Decrease 2% every tick
                    gui_update_master_timer(current_question_timer);
                } else {
                    current_state = GAME_STATE_RESULTS;
                    gui_load_master_leaderboard_screen();
                }
            }
        } else if(current_role == GAME_ROLE_SLAVE) {
            // Check for incoming questions from Master via ESP-NOW
            // If question received:
            // current_state = GAME_STATE_QUESTION;
            // gui_load_slave_answer_screen();
        }

        osDelay(100); // Run every 100ms
    }
}

void StartTask_COMM(void *argument) {
    for(;;) {
        osDelay(100);
    }
}

void StartTask_GAME(void *argument) {
    game_init();
    game_task_func(argument);
}
