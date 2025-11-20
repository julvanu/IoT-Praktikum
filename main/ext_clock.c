#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <ds3231.h>
#include <i2cdev.h>
#include <string.h>

#include "esp_log.h"
#include "main.h"
#include "time.h"

i2c_dev_t init_ext_clock(void) {
    ESP_LOGI("RTC", "Initializing external clock...");
    i2cdev_init();
    i2c_dev_t dev;
    memset(&dev, 0, sizeof(i2c_dev_t));
    ESP_ERROR_CHECK(ds3231_init_desc(&dev, 0, CLOCK_SDA_PIN, CLOCK_SCL_PIN));
    return dev;
}

time_t get_time_ext_clock_no_init(i2c_dev_t dev) {
    struct tm time = { 0 };
    if (ds3231_get_time(&dev, &time) != ESP_OK) {
        ESP_LOGE("RTC", "Could not get time\n");
    }
    // ESP_LOGI("INFO", "External clock: %04d-%02d-%02d %02d:%02d:%02d\n", time.tm_year + 1900, time.tm_mon + 1, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec);
    
    time_t timestamp = mktime(&time);
    if (timestamp == -1) {
        ESP_LOGE("RTC", "Getting timestamp from external clock failed\n");
    } 
    return timestamp;
}

time_t get_time_ext_clock(void) {
    i2c_dev_t dev = init_ext_clock();

    return get_time_ext_clock_no_init(dev);
}