#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "uart_task.h"
#include "espnow_task.h"

void app_main()
{
    // Initialize NVS (required for WiFi and ESP-NOW)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    uart_task_init();
    espnow_task_init();

    // Send ESP_READY to STM32
    protocol_msg_t ready_msg;
    ready_msg.cmd = CMD_ESP_READY;
    ready_msg.len = 0;
    uart_task_send(&ready_msg);
}
