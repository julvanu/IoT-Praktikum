#include "esp_log.h"
#include "esp_system.h"

void sleep_minutes (int minutes) {
    esp_sleep_enable_timer_wakeup(minutes * 60000000);
    ESP_LOGI("progress", "Going to sleep for %d minutes, then try again", minutes);
    esp_deep_sleep_start();
}