#pragma once
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"

void start_mqtt(void);
// void addPIREventToMQTT(char msg[], char roomID[]);
void addPIREvent(char roomID[]);
void sendPIReventToMQTT(char roomID[]);
void sendBatteryStatusToMQTT(void);
// void handleDoorEvent();
// void sendDoorEventsToMQTT(time_t door_opened, time_t door_closed);
void sendDoorEventToMQTT(char eventType[]);