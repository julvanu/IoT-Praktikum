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
#include "identify_device.h"
#include "init_logging.h"


void app_main() {
  // 1 == corridor | 2 == bathroom
  static RTC_DATA_ATTR int device_id = 0; 

  init_logging();  

  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  esp_sleep_wakeup_cause_t wakeup_cause = esp_sleep_get_wakeup_cause();
  if (wakeup_cause == ESP_SLEEP_WAKEUP_EXT0 || wakeup_cause == ESP_SLEEP_WAKEUP_EXT1)  {
    ESP_LOGI("INFO", "External wakeup trigger");
  } else { //ESP_SLEEP_WAKEUP_UNDEFINED
    ESP_LOGI("INFO", "FLASH wakeup");
    device_id = identify_device();
    if(atoi(DEVICE_ID) != device_id) {
      ESP_LOGE("ALARM", "Wrong configured DEVICE_ID in main.h!");
    }
  }

  // ESP_LOGI("progress", "Starting Wifi");
  // start_wifi();

  // ESP_LOGI("progress", "Starting Clock");
  // start_clock();

  // ESP_LOGI("progress", "Starting MQTT");
  // start_mqtt();

  // enable PIN for PIR sensor
  ESP_ERROR_CHECK(gpio_set_direction(PIR_PIN, GPIO_MODE_INPUT));
  ESP_ERROR_CHECK(rtc_gpio_pulldown_en(PIR_PIN));

  if (device_id == 1) {
    // ---------------- DEVICE: corridor ------------------------------
    char roomID[] = "corridor";
    ESP_LOGI("INFO", "DEVICE: %s", roomID);

    // Enable PIN for door sensor
    ESP_ERROR_CHECK(gpio_input_enable(DOOR_PIN));
    ESP_ERROR_CHECK(gpio_set_direction(DOOR_PIN, GPIO_MODE_INPUT));
    ESP_ERROR_CHECK(rtc_gpio_pulldown_en(DOOR_PIN));
    
    // Battery status code
    getRSOC();
    ESP_LOGI("progress", "Sending battery status to MQTT");
    sendBatteryStatusToMQTT();

    uint64_t wakeup_mask=esp_sleep_get_ext1_wakeup_status();
    if ((wakeup_mask & (1ULL<<PIR_PIN)) !=0) {
      ESP_LOGI("INFO", "WAKE UP: Due to PIR event.");
    
      // PIR sensor code
      // sendPIReventToMQTT(roomID);
      addPIREvent(roomID);
    } else {
      ESP_LOGI("INFO", "WAKE UP: Due to opened door.");
      sendDoorEventToMQTT("open");

      if (gpio_get_level(DOOR_PIN)==1) {
        ESP_LOGI("INFO", "Waiting on the door to close.");
        while (gpio_get_level(DOOR_PIN)==1){
          vTaskDelay(pdMS_TO_TICKS(1000));
        }
      }

      sendDoorEventToMQTT("closed");
    }
    // ------------------------------ WAKE UP registration --------------------------------------------------
    ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup(PIN_MASK,ESP_EXT1_WAKEUP_ANY_HIGH));
  } else if (device_id == 2) {
    // ---------------- DEVICE: bathroom ------------------------------
    char roomID[] = "bathroom";
    ESP_LOGI("INFO", "DEVICE: %s", roomID);
    
    // PIR sensor code
    // sendPIReventToMQTT(roomID);
    addPIREvent(roomID);
    ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(PIR_PIN, 1));
  } else {
    // ---------------- Error handling: Unknown MAC address -----------
    ESP_LOGI("INFO", "DEVICE: UNKNOWN");
  }

  // sleep and wake up
  ESP_LOGI("progress", "Installing wakeup for PIR sensor");
  while (gpio_get_level(PIR_PIN)==1){
    vTaskDelay(pdMS_TO_TICKS(1000));
  }

  ESP_LOGI("progress", "Going to sleep...");
  esp_deep_sleep_start();
}