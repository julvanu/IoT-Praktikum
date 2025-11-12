#include "esp_log.h"
#include "esp_system.h"
#include "esp_sleep.h"
#include "esp_pm.h"

void configPM() {
    esp_pm_config_esp32_t pm_config = {
    .max_freq_mhz = 160,
    .min_freq_mhz = 160, //DFS, enable in menucofig in Power Management
    .light_sleep_enable = true //automatic light sleep, enable via menuconfig in FreeRTOS
    };
    ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
    ESP_LOGI("INFO", "Frequency config successful.");
    // esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER); // Prof. Gerndt said this would help but it did not seem like it does
} 