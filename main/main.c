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
#include "esp_mac.h"


void app_main() {
  ESP_LOGI("progress", "[APP] Free memory: %d bytes", esp_get_free_heap_size());
  ESP_LOGI("progress", "[APP] IDF version: %s", esp_get_idf_version());

  esp_log_level_set("*", ESP_LOG_ERROR);
  esp_log_level_set("mqtt", ESP_LOG_INFO);
  esp_log_level_set("progress", ESP_LOG_INFO);
  esp_log_level_set("gauge", ESP_LOG_INFO);
  esp_log_level_set("INFO", ESP_LOG_INFO);
  esp_log_level_set("DEBUG", ESP_LOG_DEBUG);

  // getRSOC(); //only if battery!
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // MAC address retrieval
  ESP_LOGI("INFO", "Attempting to retrieve MAC-address...");
  unsigned char mac_base[6] = {0};
  esp_read_mac(mac_base, ESP_MAC_WIFI_STA);
  unsigned char mac_address[18];
  snprintf(mac_address, 18, "%02x:%02x:%02x:%02x:%02x:%02x", mac_base[0],mac_base[1],mac_base[2],mac_base[3],mac_base[4],mac_base[5]);
  ESP_LOGI("INFO", "MAC-address: %s", mac_address);

  ESP_LOGI("progress", "Starting Wifi");
  start_wifi();

  ESP_LOGI("progress", "Starting Clock");
  start_clock();

  ESP_LOGI("progress", "Starting MQTT");
  start_mqtt();

  // Init MQTT message
  char msg[1500];

  // Identity check
  unsigned char mac_corridor[] = "64:b7:08:6e:ae:bc";
  unsigned char mac_bathroom[] = "94:b9:7e:54:d3:00";

  if (0 == memcmp ( mac_address, mac_corridor, sizeof(mac_address) )) {
    // ---------------- DEVICE: corridor ------------------------------
    char roomID[] = "corridor";
    ESP_LOGI("INFO", "DEVICE: %s", roomID);
    
    // Battery status code
    ESP_LOGI("progress", "Sending battery status to MQTT");
    sendBatteryStatusToMQTT();
    
    // PIR sensor code
    ESP_LOGI("progress", "Sending PIR event to MQTT");
    addPIREventToMQTT(msg, roomID);
    addPIREventToMQTT(msg, roomID);
    ESP_LOGD("DEBUG", "Accumulated double MQTT msg: %s", msg);
    sendPIReventToMQTT(roomID);
  } else if (0 == memcmp ( mac_address, mac_bathroom, sizeof(mac_address) )) {
    // ---------------- DEVICE: bathroom ------------------------------
    char roomID[] = "bathroom";
    ESP_LOGI("INFO", "DEVICE: %s", roomID);
    
    // PIR sensor code
    ESP_LOGI("progress", "Sending PIR event to MQTT");
    sendPIReventToMQTT(roomID);
  } else {
    // ---------------- Error handling: Unknown MAC address -----------
    ESP_LOGI("INFO", "DEVICE: UNKNOWN");
  }

  ESP_ERROR_CHECK(gpio_set_direction(PIR_PIN, GPIO_MODE_INPUT));
  ESP_ERROR_CHECK(rtc_gpio_pulldown_en(PIR_PIN));

  // sleep and wake up
  ESP_LOGI("progress", "Installing wakeup");
  while (gpio_get_level(PIR_PIN)==1){
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
  ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(PIR_PIN, 1));
  ESP_LOGI("progress", "Going to sleep");
  esp_deep_sleep_start();
}