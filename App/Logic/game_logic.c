#include "game_logic.h"
#include "gui_app.h"
#include "cmsis_os.h" // For FreeRTOS functions like osDelay
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "protocol.h"
#include "usart.h"
#include "rs232.h"

// Game state variables
extern osMessageQueueId_t commQueueHandle;
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
// Helper to send command to ESP8266
void comm_send_cmd(uint8_t cmd, uint8_t *payload, uint8_t len) {
    protocol_msg_t msg;
    msg.cmd = cmd;
    msg.len = len;
    if (len > 0 && payload != NULL) {
        memcpy(msg.payload, payload, len);
    }
    
    uint8_t tx_buf[MAX_PAYLOAD + 4];
    int frame_len = protocol_build_frame(tx_buf, &msg);
    debug_log("TX ESP CMD:%02X, LEN:%d\r\n", cmd, len);
    HAL_UART_Transmit(&huart2, tx_buf, frame_len, 100);
}

// Master Callbacks
void game_master_start_quiz(void) {
    if(current_role == GAME_ROLE_MASTER && current_state == GAME_STATE_LOBBY) {
        current_state = GAME_STATE_QUESTION;
        comm_send_cmd(CMD_START_QUIZ, NULL, 0);
        
        const char *q = "What is the capital of France?";
        comm_send_cmd(CMD_BROADCAST_QUESTION, (uint8_t*)q, strlen(q) + 1);
        
        gui_load_master_question_screen(q);
        current_question_timer = 100; // 100%
        gui_update_master_timer(current_question_timer);
    }
}

void game_master_next_question(void) {
    if(current_role == GAME_ROLE_MASTER) {
        current_state = GAME_STATE_QUESTION;
        const char *q = "Next question...";
        comm_send_cmd(CMD_BROADCAST_QUESTION, (uint8_t*)q, strlen(q) + 1);
        gui_load_master_question_screen(q);
    }
}

// Slave Callbacks
void game_slave_scan_hosts(void) {
    if(current_role == GAME_ROLE_SLAVE) {
        comm_send_cmd(CMD_SCAN_HOSTS, NULL, 0);
    }
}

void game_slave_join_host(int host_id) {
    if(current_role == GAME_ROLE_SLAVE) {
        current_state = GAME_STATE_LOBBY;
        // host_id is just a mock ID, for now we will just use a fake MAC for the parameter
        // In reality we should map host_id back to MAC.
        uint8_t fake_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, host_id};
        comm_send_cmd(CMD_JOIN_HOST, fake_mac, 6);
        gui_load_slave_waiting_screen();
    }
}

void game_slave_submit_answer(int answer_idx) {
    if(current_role == GAME_ROLE_SLAVE && current_state == GAME_STATE_QUESTION) {
        current_state = GAME_STATE_FEEDBACK;
        uint8_t ans = (uint8_t)answer_idx;
        comm_send_cmd(CMD_SUBMIT_ANSWER, &ans, 1);
        // We will wait for FEEDBACK_RECEIVED to show feedback screen
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
        }
        // Process incoming ESP-NOW messages from commQueue
        protocol_msg_t *rx_msg;
        if (osMessageQueueGet(commQueueHandle, &rx_msg, NULL, 0) == osOK) {
            debug_log("RX ESP CMD:%02X, LEN:%d\r\n", rx_msg->cmd, rx_msg->len);
            if (current_role == GAME_ROLE_SLAVE) {
                if (rx_msg->cmd == CMD_HOST_FOUND) {
                    char name[32] = {0};
                    if (rx_msg->len > 6) {
                        int name_len = rx_msg->len - 6;
                        if (name_len > 31) name_len = 31;
                        memcpy(name, &rx_msg->payload[6], name_len);
                    } else {
                        strcpy(name, "Unknown Host");
                    }
                    // Use last byte of MAC as host_id for now
                    gui_slave_add_host_to_list(name, rx_msg->payload[5]);
                } else if (rx_msg->cmd == CMD_QUESTION_RECEIVED) {
                    current_state = GAME_STATE_QUESTION;
                    gui_load_slave_answer_screen();
                } else if (rx_msg->cmd == CMD_FEEDBACK_RECEIVED) {
                    bool correct = rx_msg->payload[0];
                    current_score = (rx_msg->payload[1] << 8) | rx_msg->payload[2];
                    gui_load_slave_feedback_screen(correct, current_score);
                }
            } else if (current_role == GAME_ROLE_MASTER) {
                if (rx_msg->cmd == CMD_PLAYER_JOINED) {
                    connected_players++;
                    gui_update_master_lobby_count(connected_players);
                } else if (rx_msg->cmd == CMD_ANSWER_RECEIVED) {
                    // Logic to handle answers
                    uint8_t ans = rx_msg->payload[6];
                    bool correct = (ans == 1); // Mock validation
                    
                    uint8_t feedback[3];
                    feedback[0] = correct;
                    feedback[1] = 0; // Mock score high byte
                    feedback[2] = correct ? 100 : 0; // Mock score low byte
                    
                    uint8_t fb_payload[9];
                    memcpy(fb_payload, rx_msg->payload, 6); // MAC
                    memcpy(&fb_payload[6], feedback, 3);
                    
                    comm_send_cmd(CMD_SEND_FEEDBACK, fb_payload, 9);
                }
            }
            // No vPortFree here since we use a static pool
        }

        osDelay(100); // Run every 100ms
    }
}

static protocol_parser_t parser;
static uint8_t uart2_rx_buf[1];

#define MSG_POOL_SIZE 4
static protocol_msg_t msg_pool[MSG_POOL_SIZE];
static uint8_t pool_idx = 0;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        if (protocol_parse_byte(&parser, uart2_rx_buf[0], &msg_pool[pool_idx])) {
            protocol_msg_t *msg_ptr = &msg_pool[pool_idx];
            if (osMessageQueuePut(commQueueHandle, &msg_ptr, 0, 0) == osOK) {
                pool_idx = (pool_idx + 1) % MSG_POOL_SIZE;
            }
        }
        HAL_UART_Receive_IT(&huart2, uart2_rx_buf, 1);
    }
}

void StartTask_COMM(void *argument) {
    protocol_parser_init(&parser);
    HAL_UART_Receive_IT(&huart2, uart2_rx_buf, 1);
    for(;;) {
        osDelay(1000);
    }
}

void StartTask_GAME(void *argument) {
    game_init();
    game_task_func(argument);
}
