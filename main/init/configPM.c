#include "esp_log.h"
#include "esp_system.h"
#include "esp_sleep.h"
#include "esp_pm.h"
#include "stdbool.h"

// Default: 160, 160, true
void configPM(int max_freq_mhz, int min_freq_mhz, bool light_sleep_enable) {
    esp_pm_config_esp32_t pm_config = {
        .max_freq_mhz = max_freq_mhz,
        .min_freq_mhz = min_freq_mhz, //DFS, enable in menucofig in Power Management
        .light_sleep_enable = light_sleep_enable //automatic light sleep, enable via menuconfig in FreeRTOS
    };
    ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
    ESP_LOGI("INFO", "Frequency config successful.");
    // esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER); // Prof. Gerndt said this would help but it did not seem like it does
} 