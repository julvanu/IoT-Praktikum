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

int flash_init() {
    ESP_LOGI("INFO", "FLASH wakeup\n");

    int device_id = identify_device();
    if(atoi(DEVICE_ID) != device_id) {
        ESP_LOGE("ERROR", "Wrong DEVICE_ID in main.h! DEVICE_ID is defined as %d, but the actual detected device_id is %d.", atoi(DEVICE_ID), device_id);
        esp_system_abort("System abort due to wrong DEVICE_ID in main.h!");
    }

    initialize();

    // device specific setup
    if (device_id == 1) {
        // DEVICE: corridor
        ESP_LOGI("INFO", "Sensor setup for DEVICE corridor...");
        setup_PIR();
        setup_door();
        setup_ext1_any_wakeup();
    } else if (device_id == 2 || device_id == 4 || device_id == 5 || device_id == 6) {
        // DEVICE: bathroom | kitchen | livingroom | bedroom
        char* roomID = get_device_name_by_id(device_id);
        ESP_LOGI("INFO", "Sensor setup for DEVICE %s...", roomID);
        setup_PIR();
        setup_ext0_PIR_wakeup();      
    } else {
        ESP_LOGE("ERROR", "Unknown device detected. The function identify_device returned device_id %d.", device_id);
        abort();
    }
    
    init_periodic_wakeup_timer();

    ESP_LOGI("INFO", "Setup complete");

    return device_id;
}