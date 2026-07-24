#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "uart_task.h"
#include "espnow_task.h"
#include "driver/pwm.h"
#include "esp_debug.h"

#define LED_PIN 2

static void pwm_blink_task(void *arg) {
    const uint32_t pin_num[1] = {LED_PIN};
    uint32_t duties[1] = {1000}; // Start fully off (active low)
    float phase[1] = {0};
    
    // Initialize PWM with 1kHz frequency
    pwm_init(1000, duties, 1, pin_num);
    pwm_set_phases(phase);
    pwm_start();
    
    while(1) {
        pwm_set_duty(0, 950);
        pwm_start();
        vTaskDelay(pdMS_TO_TICKS(1000));
        
        // Blink OFF
        pwm_set_duty(0, 1000);
        pwm_start();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

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
    
    // Start blink task
    xTaskCreate(pwm_blink_task, "pwm_blink_task", 1024, NULL, 5, NULL);
    
    // Test debug log
    debug_log("ESP8266 initialized successfully. Version: %d", 1);
}
