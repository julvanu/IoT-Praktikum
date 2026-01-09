#include "esp_log.h"
#include "esp_system.h"

void sleep_minutes (int minutes) {
    esp_sleep_enable_timer_wakeup(60000000LL * minutes);  // Add ONE ZERO again!
    ESP_LOGI("progress", "Going to sleep for %d minute(s)", minutes);
    esp_light_sleep_start();
    ESP_LOGI("progress", "Woke up from light sleep");
}