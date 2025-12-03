#pragma once
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "time.h"

void start_mqtt(void);
// void addPIREventToMQTT(char msg[], char roomID[]);
void sendPIREvents(char roomID[]);
int addPIREvent(void);
void sendBatteryStatusToMQTT(void);
void sendDoorEventToMQTT(time_t time, char eventType[]);
void send_periodic_data(int device_id);