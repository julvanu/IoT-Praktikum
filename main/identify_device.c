#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "esp_system.h"
#include "esp_mac.h"
#include "main.h"

int identify_device() {
    int device_id = 0;
    
    // MAC address retrieval
    ESP_LOGI("INFO", "Attempting to retrieve MAC-address...");
    unsigned char mac_base[6] = {0};
    esp_read_mac(mac_base, ESP_MAC_WIFI_STA);
    unsigned char mac_address[18];
    snprintf(mac_address, 18, "%02x:%02x:%02x:%02x:%02x:%02x", mac_base[0],mac_base[1],mac_base[2],mac_base[3],mac_base[4],mac_base[5]);
    ESP_LOGI("INFO", "MAC-address: %s", mac_address);

    if (0 == memcmp ( mac_address, MAC_CORRIDOR, sizeof(mac_address) )) {
      device_id = 1;
    } else if (0 == memcmp ( mac_address, MAC_BATHROOM, sizeof(mac_address) )) {
      device_id = 2;
    } else if (0 == memcmp ( mac_address, MAC_KITCHEN, sizeof(mac_address) )) {
      device_id = 4;
    }
    return device_id;
}

char* get_device_name_by_id(int device_id) {
    switch (device_id) {
        case 1:
            return "corridor";
        case 2:
            return "bathroom";
        case 4:
            return "kitchen";
        default:
            return NULL;
    }
}