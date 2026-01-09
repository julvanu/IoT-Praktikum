#include "main.h"

#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "identify_device.h"
#include "sensor_setup.h"
#include "initialize.h"
#include "periodic_wakeup_timer.h"

void flash_init() {
    ESP_LOGI("INFO", "FLASH wakeup\n");

    identify_device();

    initialize();

    // device specific setup
    if (ESP_DEVICE_ID == 1) {
        // DEVICE: corridor
        ESP_LOGI("INFO", "Sensor setup for DEVICE corridor...");
        setup_PIR();
        setup_door();
    } else if (ESP_DEVICE_ID == 2 || ESP_DEVICE_ID == 4 || ESP_DEVICE_ID == 5 || ESP_DEVICE_ID == 6) {
        // DEVICE: bathroom | kitchen | livingroom | bedroom
        char* roomID = get_device_name_by_id();
        ESP_LOGI("INFO", "Sensor setup for DEVICE %s...", roomID);
        setup_PIR();
    } else {
        ESP_LOGE("ERROR", "Unknown device detected. The function identify_device returned device_id %d.", ESP_DEVICE_ID);
        abort();
    }
    ESP_LOGI("INFO", "Setup complete");

    return;
}