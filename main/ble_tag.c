#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "ble_tag.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_defs.h"
// #include "esp_ibeacon_api.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

static SemaphoreHandle_t scan_done_sem;
bool found_device = false;
int8_t found_rssi = 127;

static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    esp_err_t err;
    switch (event) {
        case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
            // Scan start complete event to indicate scan start successfully or failed
            if ((err = param->scan_start_cmpl.status) != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(BLE_LOGGING_TAG, "Scanning start failed, error %s", esp_err_to_name(err));
            }
            else {
                ESP_LOGI(BLE_LOGGING_TAG, "Scanning start successfully");
            }
            break;
        case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
            if ((err = param->scan_stop_cmpl.status) != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(BLE_LOGGING_TAG, "Scanning stop failed, error %s", esp_err_to_name(err));
            }
            else {
                ESP_LOGI(BLE_LOGGING_TAG, "Scanning stop successfully");
            }
            break;
        case ESP_GAP_BLE_SCAN_RESULT_EVT:
            esp_ble_gap_cb_param_t *scan_result = (esp_ble_gap_cb_param_t *)param;
            switch (scan_result->scan_rst.search_evt) {
                case ESP_GAP_SEARCH_INQ_RES_EVT:
                    int8_t rssi = scan_result->scan_rst.rssi;
                    uint8_t *bda = scan_result->scan_rst.bda;
                    char mac_address[18];
                    addr_to_str(bda, mac_address, sizeof(mac_address));
                    if (memcmp(mac_address, MAC_BLE_TAG, sizeof(mac_address)) == 0) {
                        found_device = true;
                        found_rssi = rssi;
                        ESP_LOGI(BLE_LOGGING_TAG, "Found target by address %s RSSI=%d", mac_address, rssi);
                    } else {
                        ESP_LOGE(BLE_LOGGING_TAG, "Unknown BLE tag found with address: %s", mac_address);
                    }
                    /* Search for BLE iBeacon Packet */
                    // if (esp_ble_is_ibeacon_packet(scan_result->scan_rst.ble_adv, scan_result->scan_rst.adv_data_len)) {
                    //     esp_ble_ibeacon_t *ibeacon_data = (esp_ble_ibeacon_t *)(scan_result->scan_rst.ble_adv);
                    //     ESP_LOGI(BLE_LOGGING_TAG, "----------iBeacon Found----------");
                    //     ESP_LOGI(BLE_LOGGING_TAG, "Device address: " ESP_BD_ADDR_STR "", ESP_BD_ADDR_HEX(scan_result->scan_rst.bda));
                    //     ESP_LOGI(BLE_LOGGING_TAG, "For manual check, the predefined address is: %s", MAC_BLE_TAG);
                    //     ESP_LOG_BUFFER_HEX("BLE_TAG: Proximity UUID", ibeacon_data->ibeacon_vendor.proximity_uuid, ESP_UUID_LEN_128);

                    //     uint16_t major = ENDIAN_CHANGE_U16(ibeacon_data->ibeacon_vendor.major);
                    //     uint16_t minor = ENDIAN_CHANGE_U16(ibeacon_data->ibeacon_vendor.minor);
                    //     ESP_LOGI(BLE_LOGGING_TAG, "Major: 0x%04x (%d)", major, major);
                    //     ESP_LOGI(BLE_LOGGING_TAG, "Minor: 0x%04x (%d)", minor, minor);
                    //     ESP_LOGI(BLE_LOGGING_TAG, "Measured power (RSSI at a 1m distance): %d dBm", ibeacon_data->ibeacon_vendor.measured_power);
                    //     ESP_LOGI(BLE_LOGGING_TAG, "RSSI of packet: %d dbm", scan_result->scan_rst.rssi);
                    // }
                    break;
                case ESP_GAP_SEARCH_INQ_CMPL_EVT:
                    // scan completed
                    ESP_LOGI(BLE_LOGGING_TAG, "Scan complete. found_device=%d rssi=%d", found_device, found_rssi);
                    xSemaphoreGive(scan_done_sem);
                    break;
            }
    }
}

bool check_ble_near() {
    found_device = false;
    found_rssi = 127;
    ESP_LOGI(BLE_LOGGING_TAG, "Starting BLE scan for %d seconds...", SCAN_DURATION_SECONDS);
    esp_ble_gap_start_scanning(SCAN_DURATION_SECONDS);

    // wait for scan to finish (scan_done_sem signaled in GAP callback)
    if (xSemaphoreTake(scan_done_sem, (SCAN_DURATION_SECONDS + 2) * 1000 / portTICK_PERIOD_MS) == pdTRUE) {
        if (found_device) {
            if (found_rssi >= RSSI_NEAR_THRESHOLD) {
                ESP_LOGI(BLE_LOGGING_TAG, "Device is near (RSSI >= %d)", RSSI_NEAR_THRESHOLD);
                return true;
            } else {
                ESP_LOGI(BLE_LOGGING_TAG, "Device is not near (RSSI < %d)", RSSI_NEAR_THRESHOLD);
            }
        } else {
            ESP_LOGI(BLE_LOGGING_TAG, "Target device not found during scan.");
        }
    } else {
        ESP_LOGE(BLE_LOGGING_TAG, "Timeout waiting for BLE scan to complete");
    }
    return false;
}

void register_ble_callback() {
    esp_err_t status;
    ESP_LOGI(BLE_LOGGING_TAG, "register callback");

    // Register the scan callback function to the gap module
    if ((status = esp_ble_gap_register_callback(esp_gap_cb)) != ESP_OK) {
        ESP_LOGE(BLE_LOGGING_TAG, "gap register error: %s", esp_err_to_name(status));
        return;
    }
}

void ble_init() {
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_bt_controller_init(&bt_cfg);
    esp_bt_controller_enable(ESP_BT_MODE_BLE);

    esp_bluedroid_init();
    esp_bluedroid_enable();

    // Register GAP callback
    register_ble_callback();
    
    scan_done_sem = xSemaphoreCreateBinary();
    if (!scan_done_sem) {
        ESP_LOGE(BLE_LOGGING_TAG, "Failed to create semaphore");
        return;
    }

    static esp_ble_scan_params_t ble_scan_params = {
        .scan_type = BLE_SCAN_TYPE_ACTIVE,
        .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
        .scan_filter_policy = BLE_SCAN_FILTER_ALLOW_ALL,
        .scan_interval = 0x50,
        .scan_window = 0x30,
        .scan_duplicate = BLE_SCAN_DUPLICATE_DISABLE};
    esp_ble_gap_set_scan_params(&ble_scan_params);
}