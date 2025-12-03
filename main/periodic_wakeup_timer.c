#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "time.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "ext_clock.h"
#include "mqtt.h"

#define SLEEP_SEC 86400 // 24 hours
static RTC_DATA_ATTR time_t last_data_submission;

void reset_periodic_wakeup_timer() {
    ESP_LOGI("periodic wakeup", "Resetting periodic wakeup timer");
    last_data_submission = get_time_ext_clock();
}

void continue_periodic_wakeup_timer(int device_id) {
    time_t now = get_time_ext_clock();
    double seconds_since_last = difftime(now, last_data_submission);
    double sleep_time = SLEEP_SEC - seconds_since_last;
    if (sleep_time < 30) {
        send_periodic_data(device_id);
    }
    ESP_LOGI("periodic_wakeup", "Continuing periodic wakeup timer for %d more seconds.", sleep_time);
    ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(1000000LL * sleep_time));
}

double get_seconds_since_last_submission() {
    time_t now = get_time_ext_clock();
    double diff = difftime(now, last_data_submission);
    return diff;
}

void init_periodic_wakeup_timer() {
    last_data_submission = get_time_ext_clock();
    ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(1000000LL * SLEEP_SEC));
}