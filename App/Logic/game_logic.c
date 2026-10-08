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
extern osMutexId_t uart2_tx_mutexHandle;
static GameRole_t current_role = GAME_ROLE_NONE;
static GameState_t current_state = GAME_STATE_INIT;

// Master state
// Players are never removed from these arrays, so a player's index is stable
// and doubles as its ID: slot 0 is P1, slot 1 is P2, ...
#define PLAYER_ID(idx) ((uint8_t)((idx) + 1))
static int connected_players = 0;
static uint8_t player_macs[MAX_PLAYERS][6];
static uint8_t player_scores[MAX_PLAYERS];
static bool player_active[MAX_PLAYERS];
static bool player_answered[MAX_PLAYERS]; // Nothing more to wait for from this player this question
static int current_question_index = 0;
static volatile bool master_timer_expired = false; // Set by the LVGL task, handled by the game task

// A new host scans for the lobbies already on air and takes the lowest free number,
// so every lobby gets a unique name: "BKahoot Lobby 1", "BKahoot Lobby 2", ...
#define LOBBY_NAME_PREFIX    "BKahoot Lobby "
#define HOST_NAMING_WINDOW_MS 500
#define MAX_HOST_NUMBER       32
static uint8_t host_number = 0; // 0 = not picked yet; the host stays hidden from scans until then
static volatile bool host_naming = false;
static uint32_t host_naming_deadline;
static uint32_t host_numbers_taken; // Bit n-1 is set when another host answered as lobby n

// Slave state
#define MAX_HOSTS 10
static int current_score = 0;
static uint8_t player_id = 0; // Assigned by the master via CMD_JOIN_ACK, 0 = not assigned yet
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
            memset(player_scores, 0, sizeof(player_scores));
            memset(player_active, 0, sizeof(player_active));
            host_number = 0;
            host_numbers_taken = 0;
            host_naming_deadline = osKernelGetTickCount() + HOST_NAMING_WINDOW_MS;
            host_naming = true;
            comm_send_cmd(CMD_SCAN_HOSTS, NULL, 0);
            break;
        case GAME_ROLE_SLAVE:
            current_state = GAME_STATE_INIT; // Scanning state
            discovered_hosts_count = 0;
            player_id = 0;
            break;
        default:
            host_naming = false;
            break;
    }
}

GameRole_t game_get_role(void) {
    return current_role;
}

// Master Callbacks
static int find_player_index(const uint8_t *mac) {
    for (int i = 0; i < connected_players; i++) {
        if (memcmp(player_macs[i], mac, 6) == 0) {
            return i;
        }
    }
    return -1;
}

// Helper to send command to ESP8266.
// Both the GUI task (UI callbacks) and the game task call this. Without the mutex, a
// frame sent while the other task is mid-transmit gets HAL_BUSY and is silently lost.
// Callers may already hold the LVGL lock, but nothing takes the LVGL lock while holding
// this one, so the two locks can't deadlock.
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

    osMutexAcquire(uart2_tx_mutexHandle, osWaitForever);
    HAL_StatusTypeDef status = HAL_UART_Transmit(&huart2, tx_buf, frame_len, 100);
    osMutexRelease(uart2_tx_mutexHandle);

    if (status != HAL_OK) {
        debug_log("TX ESP CMD:%02X failed, HAL status %d\r\n", cmd, status);
    }
}

// Tell a slave which ID it was assigned: [target MAC(6)][player ID(1)]
static void master_send_player_id(int player_idx) {
    uint8_t payload[7];
    memcpy(payload, player_macs[player_idx], 6);
    payload[6] = PLAYER_ID(player_idx);
    comm_send_cmd(CMD_JOIN_ACK, payload, sizeof(payload));
}

// Fill `ranking` (MAX_PLAYERS entries) with every player, highest score first.
// Players with equal scores keep their join order. Returns the number of entries.
static int master_build_ranking(PlayerRank_t *ranking) {
    int count = 0;
    for (int i = 0; i < connected_players; i++) {
        PlayerRank_t entry = { .player_id = PLAYER_ID(i), .score = player_scores[i] };
        int j = count;
        while (j > 0 && ranking[j - 1].score < entry.score) {
            ranking[j] = ranking[j - 1];
            j--;
        }
        ranking[j] = entry;
        count++;
    }
    return count;
}

// Master Callbacks
static void master_broadcast_current_question(void) {
    const char *q = question_bank[current_question_index].question;
    uint32_t duration = question_bank[current_question_index].timeLimitSec * 1000;
    uint8_t payload[MAX_PAYLOAD];
    int offset = 0;

    memset(player_answered, 0, sizeof(player_answered));
    master_timer_expired = false;

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
            PlayerRank_t ranking[MAX_PLAYERS];
            int count = master_build_ranking(ranking);
            gui_load_master_congrats_screen(ranking, count);
            return;
        }

        current_state = GAME_STATE_QUESTION;
        master_broadcast_current_question();
    }
}

// Ends the current question, because its time ran out or every player answered.
// Slaves that got no feedback show "Time's up" and the host sees the results.
// Only call this from the game task. The UI callbacks that change the role or state
// already hold the LVGL lock, so checking the state under that lock can't race them.
static void master_end_question(void) {
    app_lv_lock();
    if(current_role == GAME_ROLE_MASTER && current_state == GAME_STATE_QUESTION) {
        current_state = GAME_STATE_RESULTS;
        comm_send_cmd(CMD_TIME_UP, NULL, 0);
        PlayerRank_t ranking[MAX_PLAYERS];
        int count = master_build_ranking(ranking);
        gui_load_master_leaderboard_screen(ranking, count);
    }
    app_lv_unlock();
}

// True once every connected player has answered. It needs at least one player,
// otherwise an empty game would end every question as soon as it starts.
static bool master_all_answered(void) {
    int active_count = 0;
    for (int i = 0; i < connected_players; i++) {
        if (!player_active[i]) continue;
        if (!player_answered[i]) return false;
        active_count++;
    }
    return active_count > 0;
}

// The naming window is over: take the lowest lobby number no other host answered with.
// Only call this from the game task, see master_end_question() for the locking.
static void master_pick_lobby_name(void) {
    app_lv_lock();
    if (current_role == GAME_ROLE_MASTER && host_naming) {
        host_naming = false;
        host_number = 1;
        while (host_number < MAX_HOST_NUMBER && (host_numbers_taken & (1UL << (host_number - 1)))) {
            host_number++;
        }
        char name[32];
        snprintf(name, sizeof(name), LOBBY_NAME_PREFIX "%d", host_number);
        gui_set_master_lobby_name(name);
    }
    app_lv_unlock();
}

void game_master_timer_timeout(void) {
    // Runs in the LVGL task; the game task ends the question so that
    // "time ran out" and "everyone answered" can't both end it
    master_timer_expired = true;
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
        player_id = 0; // The host we join assigns a fresh ID
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
        // Process incoming ESP-NOW messages from commQueue. Drain it every cycle:
        // answers arrive in bursts and the queue only holds a few messages.
        protocol_msg_t *rx_msg;
        while (osMessageQueueGet(commQueueHandle, &rx_msg, NULL, 0) == osOK) {
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
                            // Each host is listed once, however many times it answers
                            bool known = false;
                            for (int i = 0; i < discovered_hosts_count; i++) {
                                if (memcmp(discovered_hosts[i], rx_msg->payload, 6) == 0) known = true;
                            }
                            if (!known && rx_msg->len >= 6 && discovered_hosts_count < MAX_HOSTS) {
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
                            gui_load_slave_answer_screen(question, options, duration_ms, player_id);
                            app_lv_unlock();
                            break;
                        }
                        case CMD_JOIN_ACK:
                            // The ESP8266 strips the target MAC, leaving just [player ID]
                            if (rx_msg->len == 1) {
                                player_id = rx_msg->payload[0];
                                debug_log("Assigned player ID: P%d\r\n", player_id);
                            }
                            break;
                        case CMD_SEND_FEEDBACK: {
                            bool correct = rx_msg->payload[0];
                            current_score = rx_msg->payload[1];
                            app_lv_lock();
                            current_state = GAME_STATE_RESULTS; // The CMD_TIME_UP that follows is ignored
                            gui_load_slave_feedback_screen(correct, current_score);
                            app_lv_unlock();
                            break;
                        }
                        case CMD_TIME_UP:
                            app_lv_lock();
                            // No feedback yet: either we didn't answer, or the answer reached
                            // the host after the question ended, so it didn't count
                            if (current_state == GAME_STATE_QUESTION || current_state == GAME_STATE_FEEDBACK) {
                                current_state = GAME_STATE_RESULTS;
                                gui_load_slave_timeup_screen();
                            }
                            app_lv_unlock();
                            break;
                        default:
                            break;
                    }
                    break;
                case GAME_ROLE_MASTER:
                    switch (rx_msg->cmd) {
                        case CMD_SCAN_HOSTS: {
                            // Answer only the board that scanned: [target MAC(6)][lobby name].
                            // A broadcast answer would also show up on every other scanning board.
                            if (rx_msg->len < 6 || host_number == 0) break;
                            uint8_t reply[6 + 32];
                            memcpy(reply, rx_msg->payload, 6);
                            int name_len = snprintf((char*)&reply[6], 32, LOBBY_NAME_PREFIX "%d", host_number) + 1;
                            comm_send_cmd(CMD_HOST_FOUND, reply, 6 + name_len);
                            break;
                        }
                        case CMD_HOST_FOUND: {
                            // Another host answered our naming scan: [host MAC(6)][lobby name]
                            size_t prefix_len = strlen(LOBBY_NAME_PREFIX);
                            if (!host_naming || rx_msg->len <= 6 + prefix_len) break;
                            rx_msg->payload[rx_msg->len - 1] = '\0';
                            const char *name = (const char*)&rx_msg->payload[6];
                            if (strncmp(name, LOBBY_NAME_PREFIX, prefix_len) != 0) break;
                            int taken = atoi(name + prefix_len);
                            if (taken >= 1 && taken <= MAX_HOST_NUMBER) {
                                host_numbers_taken |= 1UL << (taken - 1);
                            }
                            break;
                        }
                        case CMD_JOIN_HOST: {
                            if (rx_msg->len < 6) break;
                            // A slave that rejoins keeps its old slot, so its ID and score survive
                            int idx = find_player_index(rx_msg->payload);
                            if (idx < 0) {
                                if (connected_players >= MAX_PLAYERS) break;
                                idx = connected_players++;
                                memcpy(player_macs[idx], rx_msg->payload, 6);
                                app_lv_lock();
                                gui_update_master_lobby_count(connected_players);
                                app_lv_unlock();
                            }
                            player_active[idx] = true;
                            // It joined mid-question and never got this question, so don't wait for its answer
                            if (current_state == GAME_STATE_QUESTION) player_answered[idx] = true;
                            master_send_player_id(idx);
                            break;
                        }
                        case CMD_PLAYER_DROPPED: {
                            // ESP8266 gave up on this slave's link (too many missed ACKs).
                            if (rx_msg->len >= 6) {
                                int idx = find_player_index(rx_msg->payload);
                                if (idx >= 0 && player_active[idx]) {
                                    player_active[idx] = false;
                                    int active_count = 0;
                                    for (int i = 0; i < connected_players; i++) {
                                        if (player_active[i]) active_count++;
                                    }
                                    // Stop waiting for the dropped player's answer
                                    if (master_all_answered()) master_end_question();
                                }
                            }
                            break;
                        }
                        case CMD_SUBMIT_ANSWER: {
                            // Late answers don't count; the host already sent CMD_TIME_UP
                            if (current_state != GAME_STATE_QUESTION) break;
                            if (rx_msg->len >= 7 && current_question_index < question_bank_size) {
                                int player_idx = find_player_index(rx_msg->payload);
                                if (player_idx >= 0 && player_answered[player_idx]) break; // Only the first answer counts

                                uint8_t ans = rx_msg->payload[6];
                                bool correct = (ans == question_bank[current_question_index].answerIndex);

                                if (player_idx >= 0) {
                                    player_answered[player_idx] = true;
                                    if (correct) player_scores[player_idx]++;
                                }
                                uint8_t score = (player_idx >= 0) ? player_scores[player_idx] : 0;

                                uint8_t feedback[2];
                                feedback[0] = correct;
                                feedback[1] = score;

                                uint8_t fb_payload[8];
                                memcpy(fb_payload, rx_msg->payload, 6); // Target MAC
                                memcpy(&fb_payload[6], feedback, 2);

                                comm_send_cmd(CMD_SEND_FEEDBACK, fb_payload, 8);

                                // Everyone answered before the timer ran out
                                if (master_all_answered()) master_end_question();
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

        if (host_naming && (int32_t)(osKernelGetTickCount() - host_naming_deadline) >= 0) {
            master_pick_lobby_name();
        }

        if (master_timer_expired) {
            master_timer_expired = false;
            master_end_question();
        }

        osDelay(100); // Run every 100ms
    }
}

static protocol_parser_t parser;
static uint8_t uart2_rx_buf[1];

#define MSG_POOL_SIZE 10
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
