#include "settings.h"
#include "config.h"
#include "mqtt.h"
#include <ArduinoJson.h>



void loadSettings() {
    JsonDocument doc;
    doc["command"] = "get_settings";
    doc["user"] = USER;
    doc["device"] = DEVICE_ID_UUID;

    char payload[128];
    serializeJson(doc, payload, sizeof(payload));

    char topic[64];
    snprintf(topic, sizeof(topic), "config/%s/%s", USER, DEVICE_ID);

    client.publish(topic, payload);
}