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
#include "initialize.h"
#include "sensor_setup.h"
#include "flash_init.h"
#include "device_handling.h"

// #include "esp_pm.h"

// void configPM(){
//  esp_pm_config_esp32_t pm_config = {
//   .max_freq_mhz = 160,
//   .min_freq_mhz = 160, //DFS, enable in menucofig in Power Management
//   .light_sleep_enable = true //automatic light sleep, enable via menuconfig in FreeRTOS
//  };
//  ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
//  ESP_LOGI("INFO", "Frequency config successful.");
// } 


void app_main() {
  // 1 == corridor | 2 == bathroom
  static RTC_DATA_ATTR int device_id = 0;

  init_logging();

  initialize_nvs();

  esp_sleep_wakeup_cause_t wakeup_cause = esp_sleep_get_wakeup_cause();

  if (wakeup_cause == ESP_SLEEP_WAKEUP_EXT0 || wakeup_cause == ESP_SLEEP_WAKEUP_EXT1)  {
    ESP_LOGI("INFO", "External wakeup trigger");

    if (device_id == 1) {
      // ---------------- DEVICE: corridor ------------------------------
      handle_corridor();
    } else if (device_id == 2) {
      // ---------------- DEVICE: bathroom ------------------------------
      handle_bathroom();
    }
  } else { // ESP_SLEEP_WAKEUP_UNDEFINED => flash
    // configPM();
    device_id = flash_init();
  }

  ESP_LOGI("progress", "Going to sleep...");
  esp_deep_sleep_start();
}