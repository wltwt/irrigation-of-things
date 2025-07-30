#include <Arduino.h>
#include <ArduinoJson.h>
#include "mqtt.h"
#include "config.h"
#include "sensors.h"
#include "states.h"
#include "timers.h"
#include "wifisetup.h"
#include "watering.h"

PubSubClient client(espClient);


void handleWaterNow(JsonDocument& doc) {
    int secs = doc["seconds"] | 5;
    startWatering(secs);
}

void handleSetCans(JsonDocument& doc) {
    int cans = doc["cans"] | -1;
    if (cans > 0 && cans <= 10) {
        numCans = cans;
        Serial.printf("Number of cans updated to: %d\n", numCans);
    } else {
        Serial.println("Invalid can count received");
    }
}


void mqttCallback(char* topic, byte* payload, unsigned int length) {
    const char* suffix = topic + strlen(CMD_PREFIX);
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, payload, length);
    if (err) {
        Serial.print("JSON error: ");
        Serial.println(err.c_str());
        return;
    }

    if (strcmp(suffix, "/water_now") == 0) {
        handleWaterNow(doc);
    } else if (strcmp(suffix, "/set_cans") == 0) {
        handleSetCans(doc);
    } else {
        Serial.print("Unknown command: ");
        Serial.println(topic);
    }
}
  

bool reconnect() {
    if (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (client.connect("ESP32Client")) {
            Serial.println(" MQTT connected");
            char subTopic[64];
            snprintf(subTopic, sizeof(subTopic), "%s/#", CMD_PREFIX);
            client.subscribe(subTopic);
            Serial.print("Subscribed to topic: ");
            Serial.println(subTopic);
        } else {
            Serial.print("MQTT connect failed, rc=");
            Serial.print(client.state());
            Serial.println(" will retry later");
        }
    }
    return client.connected();
}

void setupMQTT() {
    client.setServer(mqtt_server, 1883);
    client.setCallback(mqttCallback);
    char subTopic[64];
    snprintf(subTopic, sizeof(subTopic), "%s/#", CMD_PREFIX);
    Serial.print("Subscribed to topic: ");
    Serial.println(subTopic);
    
    reconnect();
}

void mqttTask(void *parameter) {
  for (;;) {
    if (WiFi.status() != WL_CONNECTED) {
        setup_wifi();
    }
    if (reconnect()) {
        client.loop();
        vTaskDelay(pdMS_TO_TICKS(100));
    } else {
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
  }
}

