#include "main.h"

#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "identify_device.h"
#include "sensor_setup.h"

int flash_init() {
    ESP_LOGI("INFO", "FLASH wakeup");

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);  

    int device_id = identify_device();
    if(atoi(DEVICE_ID) != device_id) {
        ESP_LOGE("ERROR", "Wrong DEVICE_ID in main.h! DEVICE_ID is defined as %d, but the actual detected device_id is %d.", atoi(DEVICE_ID), device_id);
        abort();
    }

    // device specific setup
    if (device_id == 1) {
        // DEVICE: corridor
        ESP_LOGI("INFO", "Sensor setup for DEVICE corridor...");

        setup_PIR();
        setup_door();
        setup_battery();
    } else if (device_id == 2) {
        // DEVICE: bathroom
        ESP_LOGI("INFO", "Sensor setup for DEVICE bathroom...");

        setup_PIR();
    } else {
        ESP_LOGE("ERROR", "Unknown device detected. The function identify_device returned device_id %d.", device_id);
        abort();
    }
    ESP_LOGI("INFO", "Setup complete");

    return device_id;
}