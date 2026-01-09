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
#include "ext_clock.h"
#include "ble_tag.h"
#include "periodic_wakeup_timer.h"
#include "watchdog.h"
#include "configPM.h"


void app_main() {
  esp_sleep_wakeup_cause_t wakeup_cause = esp_sleep_get_wakeup_cause();
  configPM(160, 160, true); //max_freq_mhz, min_freq_mhz, automatic_light_sleep_enable

  init_logging();
  initialize_nvs();
  init_ext_clock();
  initialize_watchdog();

  if (wakeup_cause == ESP_SLEEP_WAKEUP_EXT0 || wakeup_cause == ESP_SLEEP_WAKEUP_EXT1)  {
    ESP_LOGI("INFO", "External wakeup trigger\n");
    ble_init();

    if (ESP_DEVICE_ID == 1) {
      // ---------------- DEVICE: corridor ---------------------------------------------------------------
      handle_corridor();
    } else if (ESP_DEVICE_ID == 2 || ESP_DEVICE_ID == 4 || ESP_DEVICE_ID == 5 || ESP_DEVICE_ID == 6) {
      // ---------------- DEVICE: bathroom | kitchen | livingroom | bedroom ------------------------------
      handle_one_PIR(ESP_DEVICE_ID);
    }

    continue_periodic_wakeup_timer(ESP_DEVICE_ID);
  } else if (wakeup_cause == ESP_SLEEP_WAKEUP_TIMER) {
    ESP_LOGI("INFO", "Timer wakeup trigger\n");
    continue_periodic_wakeup_timer(ESP_DEVICE_ID);
  } else { // ESP_SLEEP_WAKEUP_UNDEFINED => flash
    flash_init();
  }
  install_sensor_wakeup();

  ESP_LOGI("progress", "Going to sleep...");
  esp_deep_sleep_start();
}