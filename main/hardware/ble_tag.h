#pragma once

#define BLE_LOGGING_TAG "BLE_TAG"
#define MAC_BLE_TAG "d5:d9:c3:79:ec:bc"
#define SCAN_DURATION_SECONDS 2
#define RSSI_NEAR_THRESHOLD -85

bool check_ble_near();
void ble_init();