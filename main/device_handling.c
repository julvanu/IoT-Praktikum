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
#include "initialize.h"
#include "sensor_setup.h"
#include "time.h"
#include "identify_device.h"

void handle_open_door_event() {
    time_t time_opened = get_time_ext_clock();
    // No BLE check, as door opening always provides useful information (social meetings, visits/check-ups from relatives or medical staff)
    setup_door();
    initialize_data_transfer();
    // MQTT: send door data
    sendDoorEventToMQTT(time_opened, "open");
    
    if (gpio_get_level(DOOR_PIN)==1) {
        ESP_LOGI("INFO", "Waiting on the door to close.");
        while (gpio_get_level(DOOR_PIN)==1){
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
        ESP_LOGI("INFO", "Door to closed.");
    }
    // MQTT: send door data
    time_t time_closed = get_time_ext_clock();
    sendDoorEventToMQTT(time_closed, "closed");
}

void handle_corridor() {
    char roomID[] = "corridor";
    ESP_LOGI("INFO", "DEVICE: %s", roomID);

    uint64_t wakeup_mask=esp_sleep_get_ext1_wakeup_status();
    if ((wakeup_mask & (1ULL<<PIR_PIN)) !=0) {
        ESP_LOGI("INFO", "WAKE UP: Due to PIR event.\n");
        int reached_max_events = addPIREvent();
        
        // Check if door was opened during PIR event
        if (gpio_get_level(DOOR_PIN)==1) {
            handle_open_door_event();
        }

        if(reached_max_events) {
            initialize_data_transfer();
            // MQTT: send all logged PIR data
            sendPIREvents(roomID);

            sendBatteryStatusToMQTT();
        }

        // Check if door was opened during PIR event
        if (gpio_get_level(DOOR_PIN)==1) {
            handle_open_door_event();
        }
    } else {
        ESP_LOGI("INFO", "WAKE UP: Due to opened door.\n");
        handle_open_door_event();
    }
    setup_ext1_any_wakeup();
}

void handle_one_PIR(int device_id) {
    char* roomID = get_device_name_by_id(device_id);
    ESP_LOGI("INFO", "DEVICE: %s", roomID);
    int reached_max_events = addPIREvent();
    if(reached_max_events) {
        initialize_data_transfer();
        // MQTT: send all logged PIR data
        sendPIREvents(roomID);

        if(device_id == 5 || device_id == 6) { // livingroom or bedroom
            sendBatteryStatusToMQTT();
        }
    }
    setup_ext0_PIR_wakeup();
}