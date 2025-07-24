//#include <Arduino.h>
#include <Arduino.h>
//#include <PubSubClient.h>
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
    String tpc((char*)topic);
    StaticJsonDocument<128> doc;

    DeserializationError err = deserializeJson(doc, payload, length);
    if (err) {
        Serial.print("JSON error: ");
        Serial.println(err.c_str());
        return;
    }

    if (tpc == String(CMD_PREFIX) + "/water_now") {
        handleWaterNow(doc);
    } else if (tpc == String(CMD_PREFIX) + "/set_cans") {
        handleSetCans(doc);
    } else {
        Serial.println("Unknown command: " + tpc);
    }
}

  
  void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      
      client.subscribe((String(CMD_PREFIX) + "/#").c_str());
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("trying again in 5 seconds");
      delay(5000);
    }
  }
}

void setupMQTT() {
  client.setServer(mqtt_server, 1883);
  client.setCallback(mqttCallback);
  Serial.println(String("Subscribed to topic: ") + (String(CMD_PREFIX) + "/#"));
  
  reconnect();
}

void mqttTask(void *parameter) {
  for (;;) {
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

