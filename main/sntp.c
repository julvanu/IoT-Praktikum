#include <stdio.h>
#include <string.h>
#include <i2cdev.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_sntp.h"
#include "time.h"

#include "main.h"
#include "sntp.h"
#include "ext_clock.h"


void time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI("sntp", "Notification of a time synchronization event");
}

void start_clock(void)
{
    char strftime_buf[64];
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, SNTP_SERVER_NAME);
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    #ifdef CONFIG_SNTP_TIME_SYNC_METHOD_SMOOTH
        sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
    #endif
    sntp_init();

    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo = { 0 };
    do {
        int retry = 0;
        const int retry_count = 10;
        while (sntp_get_sync_status() != SNTP_SYNC_STATUS_COMPLETED && ++retry < retry_count) {
            ESP_LOGI("sntp", "Waiting for system time to be set... (%d/%d)", retry, retry_count);
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
        time(&now);
        
        setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1);
        tzset();
        localtime_r(&now, &timeinfo);
        ESP_LOGI("INFO", "Clock thinks it is year %d", timeinfo.tm_year + 1900);
    } while (timeinfo.tm_year < (2025 - 1900));

    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    ESP_LOGI("progress", "The current date/time in Germany is: %s", strftime_buf);

    // ========= Initialize ds3231 RTC =========
    i2c_dev_t dev = init_ext_clock();

    // Sync ext RTC with SNTP time
    ESP_LOGI("progress", "Syncing external clock with sntp time...");
    if (ds3231_set_time(&dev, &timeinfo) != ESP_OK) {
        ESP_LOGE("RTC", "Failed to write sntp time to rtc\n");
    }

    float temp = 0.0;
    if (ds3231_get_temp_float(&dev, &temp) != ESP_OK) {
        ESP_LOGE("RTC", "Could not get temperature\n");
    }

    now = get_time_ext_clock_no_init(dev);
    
    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);

    ESP_LOGI("INFO", "External clock: %s, %.2f°C\n", strftime_buf, temp);
}
