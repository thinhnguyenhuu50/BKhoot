#include "protocol.h"

uint8_t protocol_checksum(const uint8_t *data, uint8_t len) {
    uint8_t chk = 0;
    for (uint8_t i = 0; i < len; i++) {
        chk ^= data[i];
    }
    return chk;
}

int protocol_build_frame(uint8_t *buf, const protocol_msg_t *msg) {
    buf[0] = FRAME_START;
    buf[1] = msg->cmd;
    buf[2] = msg->len;
    
    if (msg->len > 0) {
        memcpy(&buf[3], msg->payload, msg->len);
    }
    
    buf[3 + msg->len] = protocol_checksum(&buf[1], 2 + msg->len);
    return 4 + msg->len;
}

void protocol_parser_init(protocol_parser_t *parser) {
    parser->state = 0;
    parser->cmd = 0;
    parser->len = 0;
    parser->payload_idx = 0;
}

int protocol_parse_byte(protocol_parser_t *parser, uint8_t byte, protocol_msg_t *out_msg) {
    switch (parser->state) {
        case 0: // Wait for START
            if (byte == FRAME_START) {
                parser->state = 1;
            }
            break;
            
        case 1: // CMD
            parser->cmd = byte;
            parser->state = 2;
            break;
            
        case 2: // LEN
            parser->len = byte;
            if (parser->len > MAX_PAYLOAD) {
                parser->state = 0; // Error, reset
            } else if (parser->len > 0) {
                parser->payload_idx = 0;
                parser->state = 3;
            } else {
                parser->state = 4; // Checksum directly
            }
            break;
            
        case 3: // PAYLOAD
            parser->payload[parser->payload_idx++] = byte;
            if (parser->payload_idx >= parser->len) {
                parser->state = 4;
            }
            break;
            
        case 4: // CHECKSUM
            {
                uint8_t calc_chk = parser->cmd ^ parser->len;
                for (uint8_t i = 0; i < parser->len; i++) {
                    calc_chk ^= parser->payload[i];
                }
                
                parser->state = 0; // Always reset after checksum check
                
                if (calc_chk == byte) {
                    out_msg->cmd = parser->cmd;
                    out_msg->len = parser->len;
                    if (parser->len > 0) {
                        memcpy(out_msg->payload, parser->payload, parser->len);
                    }
                    return 1;
                }
            }
            break;
            
        default:
            parser->state = 0;
            break;
    }
    
    return 0;
}
