#include "esp_debug.h"
#include "protocol.h"
#include "uart_task.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

void debug_log(const char *fmt, ...) {
    char buf[MAX_PAYLOAD];
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    if (len > 0) {
        if (len >= MAX_PAYLOAD) {
            len = MAX_PAYLOAD - 1;
        }
        
        protocol_msg_t msg;
        msg.cmd = CMD_ESP_LOG;
        msg.len = len;
        memcpy(msg.payload, buf, len);
        
        uart_task_send(&msg);
    }
}
