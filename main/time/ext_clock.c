#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <ds3231.h>
#include <i2cdev.h>
#include <string.h>

#include "esp_log.h"
#include "main.h"
#include "time.h"

i2c_dev_t dev = {};

void init_ext_clock(void) {
    ESP_LOGI("RTC", "Initializing external clock...");
    i2cdev_init();
    memset(&dev, 0, sizeof(i2c_dev_t));
    ESP_ERROR_CHECK(ds3231_init_desc(&dev, 0, CLOCK_SDA_PIN, CLOCK_SCL_PIN));
}

void ensure_ext_clock_initialized(void) {
    i2c_dev_t empty = {};   // or {}
    if (memcmp(&dev, &empty, sizeof(dev)) == 0) {
        init_ext_clock();
        if (memcmp(&dev, &empty, sizeof(dev)) == 0) {
            ESP_LOGE("RTC", "External clock initialization failed\n");
            abort();
        }
    }
}

time_t get_time_ext_clock(void) {
    ensure_ext_clock_initialized();
    struct tm time = { 0 };
    // Get time from external clock
    if (ds3231_get_time(&dev, &time) != ESP_OK) {
        ESP_LOGE("error", "RTC: Could not get time\n");
    }
    // Set correct timezone for timestamp conversion
    setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1);
    tzset();
    // Convert struct tm to time_t timestamp
    time_t timestamp = mktime(&time);
    return timestamp;
}

void set_time_ext_clock(struct tm *time) {
    ensure_ext_clock_initialized();
    if (ds3231_set_time(&dev, time) != ESP_OK) {
        ESP_LOGE("RTC", "Could not set time for external clock\n");
    }
}

float get_temp_ext_clock(void) {
    ensure_ext_clock_initialized();
    float temperature = 0.0;
    if (ds3231_get_temp_float(&dev, &temperature) != ESP_OK) {
        ESP_LOGE("RTC", "Could not get temperature from external clock\n");
    }
    return temperature;
}