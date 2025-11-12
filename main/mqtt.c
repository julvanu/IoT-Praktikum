#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "main.h"
#include "gauge.h"
#include "controlflow.h"



esp_mqtt_client_handle_t mqtt_client = NULL;
EventGroupHandle_t mqtt_event_group;
static int qos_test = 1;

const static int CONNECTED_BIT = BIT0;

#define MAX_PIR_EVENTS 6
static RTC_DATA_ATTR time_t pir_event_times[MAX_PIR_EVENTS];
static RTC_DATA_ATTR int8_t pir_event_idx = 0;

void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
  esp_mqtt_event_t *data = (esp_mqtt_event_t *)event_data;
  switch (event_id) {
    case MQTT_EVENT_CONNECTED:
      ESP_LOGI("mqtt", "MQTT_EVENT_CONNECTED\n");
      xEventGroupSetBits(mqtt_event_group, CONNECTED_BIT);
      break;

    case MQTT_EVENT_DISCONNECTED:
      ESP_LOGI("mqtt", "MQTT_EVENT_DISCONNECTED\n");
      break;

    case MQTT_EVENT_SUBSCRIBED:
      ESP_LOGI("mqtt", "MQTT_EVENT_SUBSCRIBED, msg_id=%d\n", data->msg_id);
      break;

    case MQTT_EVENT_UNSUBSCRIBED:
      ESP_LOGI("mqtt", "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d\n", data->msg_id);
      break;

    case MQTT_EVENT_PUBLISHED:
      ESP_LOGI("mqtt", "MQTT_EVENT_PUBLISHED, msg_id=%d\n", data->msg_id);
      break;

    case MQTT_EVENT_DATA:
      ESP_LOGI("mqtt", "MQTT_EVENT_DATA\n");
      ESP_LOGI("mqtt", "TOPIC=%.*s\r\n", data->topic_len, data->topic);
      ESP_LOGI("mqtt", "DATA=%.*s\r\n", data->data_len, data->data);
      ESP_LOGI("mqtt", "ID=%d, total_len=%d, data_len=%d, current_data_offset=%d\n", data->msg_id, data->total_data_len, data->data_len, data->current_data_offset);
      // count++;
      break;

    case MQTT_EVENT_ERROR:
      ESP_LOGI("mqtt", "MQTT_EVENT_ERROR\n");
      break;

    case MQTT_EVENT_BEFORE_CONNECT:
      ESP_LOGI("mqtt", "MQTT_EVENT_BEFORE_CONNECT\n");
      break;

    default:
      ESP_LOGI("mqtt", "Other event id:%d\n", data->event_id);
      break;
  }
}

void start_mqtt(void) {
  esp_mqtt_client_config_t mqtt_cfg = {};
  mqtt_cfg.broker.address.hostname = MQTT_BROKER;
  mqtt_cfg.broker.address.port = 1883;
  mqtt_cfg.broker.address.transport = MQTT_TRANSPORT_OVER_TCP;
  mqtt_cfg.session.protocol_ver = MQTT_PROTOCOL_V_3_1_1;
  mqtt_cfg.credentials.username = "JWT";
  mqtt_cfg.network.timeout_ms = 30000;
  mqtt_cfg.credentials.authentication.password = DEVICE_KEY;

  ESP_LOGI("mqtt", "[APP] Free memory: %d bytes", esp_get_free_heap_size());
  mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
  esp_mqtt_client_register_event(mqtt_client, MQTT_EVENT_ANY, mqtt_event_handler, NULL);

  mqtt_event_group = xEventGroupCreate();
  esp_mqtt_client_start(mqtt_client);
  ESP_LOGI("mqtt", "Note free memory: %d bytes", esp_get_free_heap_size());
  ESP_LOGI("mqtt", "Waiting for connection to MQTT\n");
  EventBits_t bits = xEventGroupWaitBits(mqtt_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);
  if (bits & CONNECTED_BIT) {
    ESP_LOGI("mqtt", "Connected to MQTT\n");
  } else {
    ESP_LOGI("mqtt", "Connection to MQTT failed. Going to sleep for 30 minutes, then try again.\n");
    sleep_minutes(30);    
  }
}

void sendToMQTT(char msg[], int size) {
  ESP_LOGI("mqtt", "Sent <%s> to topic %s", msg, DEVICE_TOPIC);
  auto err = esp_mqtt_client_publish(mqtt_client, DEVICE_TOPIC, msg, size, 1, 0);
  if (err == -1) {
    printf("Error while publishing to mqtt\n");
    ESP_LOGI("functions", "SendToMqttFunction terminated");
    return ESP_FAIL;
  }
}

// sends PIR events for all timestamps in pir_event_times to MQTT, then resets pir_event_idx to zero
void sendPIREvents(char roomID[]) {  
  char msg[1500];
  snprintf(msg, sizeof(msg), "{\"sensors\":[{\"name\":\"PIR\",\"values\":[");
  ESP_LOGI("INFO", "roomID: %s", roomID);

  for (int i = 0; i < pir_event_idx; i++)
  {
    size_t len = strlen(msg);
    snprintf(msg + len, sizeof(msg) - len, "{\"timestamp\":%llu, \"roomID\":\"%s\"},", pir_event_times[i] * 1000, roomID);
  }
  size_t len = strlen(msg);
  // NOTE: "len - 1" to remove the last comma in message
  snprintf(msg + len - 1, sizeof(msg) - len, "]}]}"); 

  sendToMQTT(msg, strlen(msg));
  pir_event_idx = 0;
}

// saves the timestamp of the current PIR event to pir_event_times
// returns 1 if MAX_PIR_EVENTS is reached, otherwise 0
int addPIREvent(void) {
  ESP_LOGI("progress", "Logging PIR event...");
  time_t now = 0;  
  time(&now);
  pir_event_times[pir_event_idx] = now;
  pir_event_idx += 1; 
  ESP_LOGI("INFO", "Loggend PIR event. PIR event index: %d\n", pir_event_idx);

  if (pir_event_idx == MAX_PIR_EVENTS) {
    ESP_LOGI("INFO", "Max PIR events reached\n");
    return 1;
  }
  return 0;
}

void sendPIReventToMQTT(char roomID[]) {
  time_t now = 0;
  time(&now);
  char msg[150];

  int size = snprintf(msg, sizeof(msg), "{\"sensors\":[{\"name\":\"PIR\",\"values\":[{\"timestamp\":%llu, \"roomID\":\"%s\"}]}]}", now * 1000, roomID);
  sendToMQTT(msg, size);
}

void sendBatteryStatusToMQTT(void) {
  getRSOC();
  time_t now = 0;
  time(&now);
  char msg[150];

  int size = snprintf(msg, sizeof(msg), "{\"sensors\":[{\"name\":\"battery\",\"values\":[{\"timestamp\":%llu, \"voltage\":%.1f, \"soc\":%.1f}]}]}", now * 1000, voltage, rsoc);
  
  sendToMQTT(msg, size);
}

void sendDoorEventToMQTT(char eventType[]) {
  time_t now = 0;
  time(&now);
  char msg[150];

  int size = snprintf(msg, sizeof(msg), "{\"sensors\":[{\"name\":\"door\",\"values\":[{\"timestamp\":%llu, \"eventType\":\"%s\"}]}]}", now * 1000, eventType);
  sendToMQTT(msg, size);
}
