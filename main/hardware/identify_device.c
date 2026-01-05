#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "esp_system.h"
#include "esp_mac.h"
#include "main.h"
#include "identify_device.h"

RTC_DATA_ATTR int ESP_DEVICE_ID = 0;
RTC_DATA_ATTR char ESP_DEVICE_TOPIC[9] = "";
RTC_DATA_ATTR char ESP_DEVICE_KEY[789] = "";

void addr_to_str(const uint8_t *addr, char *out, size_t out_len) {
    snprintf(out, out_len, "%02x:%02x:%02x:%02x:%02x:%02x", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
}

void identify_device() {    
    // MAC address retrieval
    ESP_LOGI("INFO", "Attempting to retrieve MAC-address...");
    unsigned char mac_base[6] = {0};
    esp_read_mac(mac_base, ESP_MAC_WIFI_STA);
    unsigned char mac_address[18];
    addr_to_str(mac_base, mac_address, sizeof(mac_address));
    ESP_LOGI("INFO", "MAC-address: %s", mac_address);

    if (0 == memcmp ( mac_address, MAC_CORRIDOR, sizeof(mac_address) )) {
      ESP_DEVICE_ID = DEVICE_ID_CORRIDOR;
      snprintf(ESP_DEVICE_TOPIC, sizeof(ESP_DEVICE_TOPIC), DEVICE_TOPIC_CORRIDOR);
      snprintf(ESP_DEVICE_KEY, sizeof(ESP_DEVICE_KEY), DEVICE_KEY_CORRIDOR);
    } else if (0 == memcmp ( mac_address, MAC_BATHROOM, sizeof(mac_address) )) {
      ESP_DEVICE_ID = DEVICE_ID_BATHROOM;
      snprintf(ESP_DEVICE_TOPIC, sizeof(ESP_DEVICE_TOPIC), DEVICE_TOPIC_BATHROOM);
      snprintf(ESP_DEVICE_KEY, sizeof(ESP_DEVICE_KEY), DEVICE_KEY_BATHROOM);
    } else if (0 == memcmp ( mac_address, MAC_KITCHEN, sizeof(mac_address) )) {
      ESP_DEVICE_ID = DEVICE_ID_KITCHEN;
      snprintf(ESP_DEVICE_TOPIC, sizeof(ESP_DEVICE_TOPIC), DEVICE_TOPIC_KITCHEN);
      snprintf(ESP_DEVICE_KEY, sizeof(ESP_DEVICE_KEY), DEVICE_KEY_KITCHEN);
    } else if (0 == memcmp ( mac_address, MAC_LIVINGROOM, sizeof(mac_address) )) {
      ESP_DEVICE_ID = DEVICE_ID_LIVINGROOM;
      snprintf(ESP_DEVICE_TOPIC, sizeof(ESP_DEVICE_TOPIC), DEVICE_TOPIC_LIVINGROOM);
      snprintf(ESP_DEVICE_KEY, sizeof(ESP_DEVICE_KEY), DEVICE_KEY_LIVINGROOM);
    } else if (0 == memcmp ( mac_address, MAC_BEDROOM, sizeof(mac_address) )) {
      ESP_DEVICE_ID = DEVICE_ID_BEDROOM;
      snprintf(ESP_DEVICE_TOPIC, sizeof(ESP_DEVICE_TOPIC), DEVICE_TOPIC_BEDROOM);
      snprintf(ESP_DEVICE_KEY, sizeof(ESP_DEVICE_KEY), DEVICE_KEY_BEDROOM);
    } else {
      ESP_LOGE("ERROR", "System abort due to not recognized MAC-address!");
      abort();
    }
    return;
}

char* get_device_name_by_id() {
    switch (ESP_DEVICE_ID) {
        case 1:
            return "corridor";
        case 2:
            return "bathroom";
        case 4:
            return "kitchen";
        case 5:
            return "livingroom";
        case 6:
            return "bedroom";
        default:
            return NULL;
    }
}