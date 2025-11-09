
#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "esp_system.h"

void init_logging() {
  ESP_LOGI("progress", "[APP] Free memory: %d bytes", esp_get_free_heap_size());
  ESP_LOGI("progress", "[APP] IDF version: %s", esp_get_idf_version());

  esp_log_level_set("*", ESP_LOG_ERROR);
  esp_log_level_set("mqtt", ESP_LOG_INFO);
  esp_log_level_set("progress", ESP_LOG_INFO);
  esp_log_level_set("gauge", ESP_LOG_INFO);
  esp_log_level_set("INFO", ESP_LOG_INFO);
}