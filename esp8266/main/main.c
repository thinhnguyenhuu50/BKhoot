#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/pwm.h"

#define BLINK_GPIO 2

void blink_task(void *pvParameter)
{
    // Initialize PWM on GPIO2
    const uint32_t pin_num[1] = {BLINK_GPIO};
    uint32_t duties[1] = {0}; // Start with 0 duty
    
    // PWM period 1000us (1kHz)
    pwm_init(1000, duties, 1, pin_num);
    
    int blink_state = 0;
    while(1) {        
        if (blink_state) {
            // Low brightness: ESP8266 GPIO2 LED is usually active-low.
            // Duty = 950 means HIGH for 95% of the time, and LOW (ON) for 5%.
            // If your LED is active-high, change this to 50 instead of 950.
            pwm_set_duty(0, 950); 
        } else {
            // OFF: 1000 means HIGH (OFF for active-low) 100% of the time.
            // If your LED is active-high, change this to 0 instead of 1000.
            pwm_set_duty(0, 1000);
        }
        pwm_start();
        
        blink_state = !blink_state;
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main()
{
    xTaskCreate(&blink_task, "blink_task", 2048, NULL, 5, NULL);
}
