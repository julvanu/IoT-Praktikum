#pragma once


// #define EXAMPLE_ESP_WIFI_SSID      "CAPS-Seminar-Room"
// #define EXAMPLE_ESP_WIFI_PASS      "caps-schulz-seminar-room-wifi"
// #define SNTP_SERVER_NAME           "ntp1.in.tum.de"

#define EXAMPLE_ESP_WIFI_SSID      "Raum der Wuensche"
#define EXAMPLE_ESP_WIFI_PASS      "2Affenimboot"
#define SNTP_SERVER_NAME           "pool.ntp.org"

#define MQTT_BROKER                "192.168.178.41" // your RPi IP address (WLAN: .39, LAN: .41)

#define PIR_PIN 27 
#define DOOR_PIN 32
#define PIN_MASK ((1ULL<<PIR_PIN)|(1ULL<<DOOR_PIN))
#define CLOCK_SCL_PIN 19
#define CLOCK_SDA_PIN 18

#define WATCHDOG_TIMEOUT_MS 1800000