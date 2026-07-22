#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define BLINK_GPIO 2

void blink_task(void *pvParameter)
{
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set
    io_conf.pin_bit_mask = (1ULL << BLINK_GPIO);
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    int blink_state = 0;
    while(1) {
        /* Blink off (output low) */
        printf("Turning the LED %s!\n", blink_state ? "ON" : "OFF");
        gpio_set_level(BLINK_GPIO, blink_state);
        blink_state = !blink_state;
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void app_main()
{
    xTaskCreate(&blink_task, "blink_task", 2048, NULL, 5, NULL);
}
