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
#include "sleep_minutes.h"

void initialize() { 
  ESP_LOGI("progress", "Starting Wifi");
  int wifi_connected = start_wifi();
  ensure_wifi_connection(wifi_connected);

  ESP_LOGI("progress", "Starting Clock");
  start_clock();
}

void initialize_data_transfer() {
  ESP_LOGI("INFO", "Preparing to send data...");
  
  ESP_LOGI("progress", "Starting Wifi");
  int wifi_connected = start_wifi();
  ensure_wifi_connection(wifi_connected);

  ESP_LOGI("progress", "Starting MQTT");
  int mqtt_connected = start_mqtt();
  ensure_mqtt_connection(mqtt_connected);
}

void ensure_wifi_connection(int wifi_connected) {
  while (!wifi_connected) {
    ESP_LOGI("progress", "Currently not connected to Wifi.");
    int sleep_duration_minutes = 3;
    ESP_LOGI("progress", "Sleeping for %d minute(s) before retrying Wifi connection...", sleep_duration_minutes);
    sleep_minutes(sleep_duration_minutes);
    ESP_LOGI("progress", "Starting Wifi (retry)");
    wifi_connected = start_wifi();
  }
}

void ensure_mqtt_connection(int mqtt_connected) {
  while (!mqtt_connected) {
    ESP_LOGI("progress", "Currently not connected to MQTT.");
    int sleep_duration_minutes = 3;
    ESP_LOGI("progress", "Sleeping for %d minute(s) before retrying MQTT connection...", sleep_duration_minutes);
    sleep_minutes(sleep_duration_minutes);
    ESP_LOGI("progress", "Starting MQTT (retry)");
    mqtt_connected = start_mqtt();
  }
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
