#include "game_logic.h"
#include "gui_app.h"
#include "cmsis_os.h" // For FreeRTOS functions like osDelay
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "protocol.h"
#include "usart.h"
#include "rs232.h"
#include "questions_data.h"

// Game state variables
extern osMessageQueueId_t commQueueHandle;
static GameRole_t current_role = GAME_ROLE_NONE;
static GameState_t current_state = GAME_STATE_INIT;

// Master state
#define MAX_PLAYERS 20
static int connected_players = 0;
static uint8_t player_macs[MAX_PLAYERS][6];
static int current_question_index = 0;

// Slave state
#define MAX_HOSTS 10
static int current_score = 0;
static uint8_t discovered_hosts[MAX_HOSTS][6];
static int discovered_hosts_count = 0;

void game_init(void) {
    current_role = GAME_ROLE_NONE;
    current_state = GAME_STATE_INIT;
}

void game_set_role(GameRole_t role) {
    current_role = role;
    switch (role) {
        case GAME_ROLE_MASTER:
            current_state = GAME_STATE_LOBBY;
            connected_players = 0;
            break;
        case GAME_ROLE_SLAVE:
            current_state = GAME_STATE_INIT; // Scanning state
            discovered_hosts_count = 0;
            break;
        default:
            break;
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
static void master_broadcast_current_question(void) {
    const char *q = question_bank[current_question_index].question;
    uint32_t duration = question_bank[current_question_index].timeLimitSec * 1000;
    uint8_t payload[MAX_PAYLOAD];
    int offset = 0;
    
    memcpy(payload + offset, &duration, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    
    int len = strlen(q) + 1;
    if(offset + len > MAX_PAYLOAD) len = MAX_PAYLOAD - offset;
    memcpy(payload + offset, q, len);
    if(len > 0) payload[offset + len - 1] = '\0';
    offset += len;
    
    for (int i = 0; i < 4; i++) {
        const char *opt = question_bank[current_question_index].options[i];
        len = strlen(opt) + 1;
        if(offset + len > MAX_PAYLOAD) len = MAX_PAYLOAD - offset;
        if (len > 0) {
            memcpy(payload + offset, opt, len);
            payload[offset + len - 1] = '\0';
            offset += len;
        }
    }
    
    comm_send_cmd(CMD_BROADCAST_QUESTION, payload, offset);
    gui_load_master_question_screen(q, duration);
}

void game_master_start_quiz(void) {
    if(current_role == GAME_ROLE_MASTER && current_state == GAME_STATE_LOBBY) {
        current_state = GAME_STATE_QUESTION;
        comm_send_cmd(CMD_START_QUIZ, NULL, 0);
        
        current_question_index = 0;
        if (question_bank_size == 0) return; // Safety check
        
        master_broadcast_current_question();
    }
}

void game_master_next_question(void) {
    if(current_role == GAME_ROLE_MASTER) {
        current_question_index++;
        if (current_question_index >= question_bank_size) {
            // End of quiz
            current_state = GAME_STATE_RESULTS;
            gui_load_master_leaderboard_screen();
            return;
        }

        current_state = GAME_STATE_QUESTION;
        master_broadcast_current_question();
    }
}

void game_master_timer_timeout(void) {
    if(current_role == GAME_ROLE_MASTER && current_state == GAME_STATE_QUESTION) {
        current_state = GAME_STATE_RESULTS;
        gui_load_master_leaderboard_screen();
    }
}

// Slave Callbacks
void game_slave_scan_hosts(void) {
    if(current_role == GAME_ROLE_SLAVE) {
        discovered_hosts_count = 0;
        comm_send_cmd(CMD_SCAN_HOSTS, NULL, 0);
    }
}

void game_slave_join_host(int host_id) {
    if(current_role == GAME_ROLE_SLAVE) {
        current_state = GAME_STATE_LOBBY;
        if (host_id >= 0 && host_id < discovered_hosts_count) {
            comm_send_cmd(CMD_JOIN_HOST, discovered_hosts[host_id], 6);
        }
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
        switch (current_role) {
            case GAME_ROLE_MASTER:
                switch (current_state) {
                    case GAME_STATE_LOBBY:
                        // Mock player joining
                        // gui_update_master_lobby_count(connected_players);
                        break;
                    case GAME_STATE_QUESTION:
                        // UI handles timer independently via lv_timer
                        break;
                    default:
                        break;
                }
                break;
            case GAME_ROLE_SLAVE:
                // UI handles timer independently via lv_timer
                break;
            default:
                break;
        }
        // Process incoming ESP-NOW messages from commQueue
        protocol_msg_t *rx_msg;
        if (osMessageQueueGet(commQueueHandle, &rx_msg, NULL, 0) == osOK) {
            debug_log("RX ESP CMD:%02X, LEN:%d\r\n", rx_msg->cmd, rx_msg->len);
            
            switch (rx_msg->cmd) {
                case CMD_ESP_READY:
                    debug_log(">>> ESP8266 IS READY! <<<\r\n");
                    break;
                case CMD_ESP_LOG: {
                    // Ensure null termination safely, though it should be a string
                    char log_buf[MAX_PAYLOAD + 1];
                    int len = rx_msg->len;
                    if(len > MAX_PAYLOAD) len = MAX_PAYLOAD;
                    memcpy(log_buf, rx_msg->payload, len);
                    log_buf[len] = '\0';
                    debug_log("[ESP8266] %s\r\n", log_buf);
                    break;
                }
                default:
                    break;
            }

            switch (current_role) {
                case GAME_ROLE_SLAVE:
                    switch (rx_msg->cmd) {
                        case CMD_HOST_FOUND: {
                            char name[32] = {0};
                            if (rx_msg->len > 6) {
                                int name_len = rx_msg->len - 6;
                                if (name_len > 31) name_len = 31;
                                memcpy(name, &rx_msg->payload[6], name_len);
                            } else {
                                strcpy(name, "Unknown Host");
                            }
                            if (discovered_hosts_count < MAX_HOSTS) {
                                memcpy(discovered_hosts[discovered_hosts_count], rx_msg->payload, 6);
                                app_lv_lock();
                                gui_slave_add_host_to_list(name, discovered_hosts_count);
                                app_lv_unlock();
                                discovered_hosts_count++;
                            }
                            break;
                        }
                        case CMD_START_QUIZ:
                            // Quiz started!
                            break;
                        case CMD_BROADCAST_QUESTION: {
                            current_state = GAME_STATE_QUESTION;
                            char question[MAX_PAYLOAD + 1] = {0};
                            const char *options[4] = {NULL, NULL, NULL, NULL};
                            char opts_buffer[4][64] = {0};
                            
                            uint32_t duration_ms = DEFAULT_QUESTION_TIMER_MS;
                            int offset = 0;
                            
                            if (rx_msg->len > 0) {
                                rx_msg->payload[rx_msg->len - 1] = '\0';
                            }
                            
                            if (rx_msg->len >= 4) {
                                memcpy(&duration_ms, rx_msg->payload, 4);
                                offset += 4;
                                
                                if (offset < rx_msg->len) {
                                    strncpy(question, (char*)&rx_msg->payload[offset], MAX_PAYLOAD);
                                    question[MAX_PAYLOAD] = '\0';
                                    offset += strlen((char*)&rx_msg->payload[offset]) + 1;
                                }
                                
                                for (int i = 0; i < 4; i++) {
                                    if (offset < rx_msg->len) {
                                        strncpy(opts_buffer[i], (char*)&rx_msg->payload[offset], 63);
                                        opts_buffer[i][63] = '\0';
                                        options[i] = opts_buffer[i];
                                        offset += strlen((char*)&rx_msg->payload[offset]) + 1;
                                    }
                                }
                            } else {
                                strcpy(question, "Question?");
                            }
                            app_lv_lock();
                            gui_load_slave_answer_screen(question, options, duration_ms);
                            app_lv_unlock();
                            break;
                        }
                        case CMD_SEND_FEEDBACK: {
                            bool correct = rx_msg->payload[0];
                            current_score = (rx_msg->payload[1] << 8) | rx_msg->payload[2];
                            app_lv_lock();
                            gui_load_slave_feedback_screen(correct, current_score);
                            app_lv_unlock();
                            break;
                        }
                        default:
                            break;
                    }
                    break;
                case GAME_ROLE_MASTER:
                    switch (rx_msg->cmd) {
                        case CMD_SCAN_HOSTS: {
                            // Send CMD_HOST_FOUND with lobby name
                            const char *lobby_name = "BKhoot Lobby";
                            comm_send_cmd(CMD_HOST_FOUND, (uint8_t*)lobby_name, strlen(lobby_name) + 1);
                            break;
                        }
                        case CMD_JOIN_HOST:
                            if (connected_players < MAX_PLAYERS && rx_msg->len >= 6) {
                                memcpy(player_macs[connected_players], rx_msg->payload, 6);
                                connected_players++;
                                app_lv_lock();
                                gui_update_master_lobby_count(connected_players);
                                app_lv_unlock();
                            }
                            break;
                        case CMD_SUBMIT_ANSWER: {
                            if (rx_msg->len >= 7) {
                                uint8_t ans = rx_msg->payload[6];
                                bool correct = (ans == 1); // Mock validation
                                
                                uint8_t feedback[3];
                                feedback[0] = correct;
                                feedback[1] = 0; // Mock score high byte
                                feedback[2] = correct ? 100 : 0; // Mock score low byte
                                
                                uint8_t fb_payload[9];
                                memcpy(fb_payload, rx_msg->payload, 6); // Target MAC
                                memcpy(&fb_payload[6], feedback, 3);
                                
                                comm_send_cmd(CMD_SEND_FEEDBACK, fb_payload, 9);
                            }
                            break;
                        }
                        default:
                            break;
                    }
                    break;
                default:
                    break;
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
