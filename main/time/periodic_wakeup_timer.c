#include <stdio.h>
#include <string.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "time.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "ext_clock.h"
#include "mqtt.h"

#define SLEEP_SEC 86400 // 24 hours
static RTC_DATA_ATTR time_t last_data_submission;
static RTC_DATA_ATTR int timer_initialized = 0;


int get_seconds_since_last_submission() {
    time_t now = get_time_ext_clock();
    int diff = (int) ceil(difftime(now, last_data_submission));

    ESP_LOGI("periodic_wakeup", "Seconds since init or last data submission: %d", diff);
    return diff;
}

void reset_periodic_wakeup_timer() {
    ESP_LOGI("periodic wakeup", "Resetting periodic wakeup timer");
    last_data_submission = get_time_ext_clock();
}

// Installs wakeup timer for the remaining time. If the time is (almost) up, sends data immediately. 
void continue_periodic_wakeup_timer() {
    if(!timer_initialized) {
        init_periodic_wakeup_timer();
        timer_initialized = 1;
    }
    int seconds_since_last = get_seconds_since_last_submission();
    int sleep_time = SLEEP_SEC - seconds_since_last;
    if (sleep_time < 30) {
        ESP_LOGI("periodic_wakeup", "Almost 24 hours since last data submission, sending data now.");
        send_periodic_data();
        sleep_time = SLEEP_SEC;
    }
    ESP_LOGI("periodic_wakeup", "Continuing periodic wakeup timer for %d more seconds.\n", sleep_time);
    ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(1000000LL * sleep_time));
}

void init_periodic_wakeup_timer() {
    ESP_LOGI("periodic_wakeup", "Initializing periodic wakeup timer.");
    last_data_submission = get_time_ext_clock();
    ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(1000000LL * SLEEP_SEC));
}