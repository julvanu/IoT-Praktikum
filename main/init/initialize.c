#include "main.h"

#include <stdio.h>
#include <string.h>

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gauge.h"
#include "mqtt.h"
#include "nvs_flash.h"
#include "sntp.h"
#include "wifi.h"
#include "esp_sleep.h"
#include "ble_tag.h"

void initialize() { 
  ESP_LOGI("progress", "Starting Wifi");
  start_wifi();

  ESP_LOGI("progress", "Starting Clock");
  start_clock();

  // TODO: SNTP => sync external clock
}

void initialize_data_transfer() {
  ESP_LOGI("INFO", "Preparing to send data...");
  
  ESP_LOGI("progress", "Starting Wifi");
  start_wifi();

  ESP_LOGI("progress", "Starting MQTT");
  start_mqtt();
}

void initialize_nvs() {
  ESP_LOGI("progress", "Initialize NVS");
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret); 
}

void init_logging() {
  ESP_LOGI("progress", "[APP] Free memory: %d bytes", esp_get_free_heap_size());
  ESP_LOGI("progress", "[APP] IDF version: %s", esp_get_idf_version());

  esp_log_level_set("*", ESP_LOG_ERROR);
  esp_log_level_set("error", ESP_LOG_ERROR);
  esp_log_level_set("mqtt", ESP_LOG_INFO);
  esp_log_level_set("progress", ESP_LOG_INFO);
  esp_log_level_set("gauge", ESP_LOG_INFO);
  esp_log_level_set("sntp", ESP_LOG_INFO);
  esp_log_level_set("RTC", ESP_LOG_INFO);
  esp_log_level_set("INFO", ESP_LOG_INFO);
  esp_log_level_set(BLE_LOGGING_TAG, ESP_LOG_INFO);
  esp_log_level_set("periodic_wakeup", ESP_LOG_INFO);
}
