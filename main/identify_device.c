#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "esp_system.h"
#include "esp_mac.h"
#include "main.h"

static void addr_to_str(const uint8_t *addr, char *out, size_t out_len) {
    snprintf(out, out_len, "%02x:%02x:%02x:%02x:%02x:%02x", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
}

int identify_device() {
    int device_id = 0;
    
    // MAC address retrieval
    ESP_LOGI("INFO", "Attempting to retrieve MAC-address...");
    unsigned char mac_base[6] = {0};
    esp_read_mac(mac_base, ESP_MAC_WIFI_STA);
    unsigned char mac_address[18];
    addr_to_str(mac_base, mac_address, sizeof(mac_address));
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