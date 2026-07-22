#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <string.h>

#define FRAME_START 0xAA
#define MAX_PAYLOAD 200

// Command IDs
#define CMD_SET_ROLE_MASTER    0x01
#define CMD_SET_ROLE_SLAVE     0x02
#define CMD_SCAN_HOSTS         0x03
#define CMD_HOST_FOUND         0x04
#define CMD_JOIN_HOST          0x05
#define CMD_JOIN_ACK           0x06
#define CMD_BROADCAST_QUESTION 0x07
#define CMD_QUESTION_RECEIVED  0x08
#define CMD_SUBMIT_ANSWER      0x09
#define CMD_ANSWER_RECEIVED    0x0A
#define CMD_SEND_FEEDBACK      0x0B
#define CMD_FEEDBACK_RECEIVED  0x0C
#define CMD_PLAYER_JOINED      0x0D
#define CMD_START_QUIZ         0x0E
#define CMD_ESP_READY          0x0F

typedef struct {
    uint8_t cmd;
    uint8_t len;
    uint8_t payload[MAX_PAYLOAD];
} protocol_msg_t;

// Helper functions
uint8_t protocol_checksum(const uint8_t *data, uint8_t len);
int protocol_build_frame(uint8_t *buf, const protocol_msg_t *msg);

// Byte parser state machine
typedef struct {
    int state;
    uint8_t cmd;
    uint8_t len;
    uint8_t payload_idx;
    uint8_t payload[MAX_PAYLOAD];
} protocol_parser_t;

void protocol_parser_init(protocol_parser_t *parser);
// Returns 1 if a full message was parsed and placed into `out_msg`
int protocol_parse_byte(protocol_parser_t *parser, uint8_t byte, protocol_msg_t *out_msg);

#endif // PROTOCOL_H
