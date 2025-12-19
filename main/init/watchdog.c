#include "esp_task_wdt.h"
#include "main.h"
#include "esp_log.h"

void initialize_watchdog() {
    esp_task_wdt_config_t twdt_config = {
        .timeout_ms = WATCHDOG_TIMEOUT_MS,
        .idle_core_mask = 0,
        .trigger_panic = true
    };
    esp_task_wdt_deinit(); // Deinitialize in case it was already initialized
    ESP_LOGI("progress", "Initializing watchdog timer with timeout %d ms", WATCHDOG_TIMEOUT_MS);
    esp_err_t err = esp_task_wdt_init(&twdt_config);
    if (err != ESP_OK) {
        ESP_LOGE("error", "Failed to initialize watchdog timer: %s", esp_err_to_name(err));
    }
    esp_task_wdt_add(NULL); // NULL = current task
}

void reset_watchdog() {
    esp_err_t err = esp_task_wdt_reset();
    if (err != ESP_OK) {
        ESP_LOGE("error", "Failed to reset watchdog timer: %s", esp_err_to_name(err));
    }
}