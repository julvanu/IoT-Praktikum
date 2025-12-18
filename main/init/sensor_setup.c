#include "main.h"

#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_sleep.h"

void setup_PIR() {
    // enable PIN for PIR sensor
    ESP_ERROR_CHECK(gpio_set_direction(PIR_PIN, GPIO_MODE_INPUT));
    ESP_ERROR_CHECK(rtc_gpio_pulldown_en(PIR_PIN));
}

void setup_battery() {
    getRSOC();
}

void setup_door() {
    // Enable PIN for door sensor
    ESP_ERROR_CHECK(gpio_input_enable(DOOR_PIN));
    ESP_ERROR_CHECK(gpio_set_direction(DOOR_PIN, GPIO_MODE_INPUT));
    ESP_ERROR_CHECK(rtc_gpio_pulldown_en(DOOR_PIN));    
}

void setup_ext0_PIR_wakeup() {
    ESP_LOGI("progress", "Installing wakeup for PIR sensor");

    // So that PIR events are not spammed
    while (gpio_get_level(PIR_PIN)==1){
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(PIR_PIN, 1));
}

void setup_ext1_any_wakeup() {
    ESP_LOGI("progress", "Installing EXT1 wakeup");
    while (gpio_get_level(PIR_PIN)==1){
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup(PIN_MASK,ESP_EXT1_WAKEUP_ANY_HIGH));
}