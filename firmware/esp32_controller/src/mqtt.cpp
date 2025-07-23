//#include <Arduino.h>
#include <Arduino.h>
//#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "mqtt.h"
#include "config.h"
#include "sensors.h"
#include "states.h"
#include "timers.h"
#include "netset.h"
#include "watering.h"



PubSubClient client(espClient);



void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String tpc((char*)topic);

  if (tpc == String(CMD_PREFIX) + "/water_now") {
    // Print payload as a char array for debugging
    for (unsigned int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
    }
    StaticJsonDocument<64> doc;
    deserializeJson(doc, payload, length);
    DeserializationError err = deserializeJson(doc, payload, length);
    if (err) {
        Serial.print("JSON error: ");
        Serial.println(err.c_str());
        return;
      }
      int secs = doc["seconds"] | 5;
      startWatering(secs);
    }
  }

  
  void reconnect() {
  while (!client.connected()) {
    Serial.print("Forsøker MQTT-tilkobling...");
    if (client.connect("ESP32Client")) {
      Serial.println("tilkoblet");
      
      client.subscribe((String(CMD_PREFIX) + "/#").c_str());
      
    } else {
      Serial.print("feilet, rc=");
      Serial.print(client.state());
      Serial.println(" prøver igjen om 5 sekunder");
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

