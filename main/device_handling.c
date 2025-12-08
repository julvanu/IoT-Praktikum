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

void handle_corridor() {
    char roomID[] = "corridor";
    ESP_LOGI("INFO", "DEVICE: %s", roomID);

    uint64_t wakeup_mask=esp_sleep_get_ext1_wakeup_status();
    if ((wakeup_mask & (1ULL<<PIR_PIN)) !=0) {
        ESP_LOGI("INFO", "WAKE UP: Due to PIR event.\n");
        
        int reached_max_events = addPIREvent();
        if(reached_max_events) {
            initialize_data_transfer();
            // MQTT: send all logged PIR data
            sendPIREvents(roomID);

            // MQTT: send battery data
            ESP_LOGI("progress", "Sending battery status to MQTT");
            sendBatteryStatusToMQTT();
        }

    } else {
        ESP_LOGI("INFO", "WAKE UP: Due to opened door.\n");
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
        }
        // MQTT: send door data
        time_t time_closed = get_time_ext_clock();
        sendDoorEventToMQTT(time_closed, "closed");
    }
    setup_ext1_any_wakeup();
}

void handle_one_PIR(char roomID[]) {
    ESP_LOGI("INFO", "DEVICE: %s", roomID);
    int reached_max_events = addPIREvent();
    if(reached_max_events) {
        initialize_data_transfer();
        // MQTT: send all logged PIR data
        sendPIREvents(roomID);
    }
    setup_ext0_PIR_wakeup();
}

void handle_bathroom() {
    char roomID[] = "bathroom";
    handle_one_PIR(roomID);
}

void handle_kitchen() {
    char roomID[] = "kitchen";
    handle_one_PIR(roomID);
}