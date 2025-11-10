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


void app_main() {
  // 1 == corridor | 2 == bathroom
  static RTC_DATA_ATTR int device_id = 0;

  init_logging();  

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
    device_id = flash_init();
  }

  ESP_LOGI("progress", "Going to sleep...");
  esp_deep_sleep_start();
}