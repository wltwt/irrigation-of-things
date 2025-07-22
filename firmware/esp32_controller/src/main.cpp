#include <Arduino.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>

#include "config.h"
#include "sensors.h"
#include "mqtt.h"

#define MOISTURE_SENSOR1_PIN 32


float moisture = 0;
float raw[NUM_MOISTURE_SENSORS];
float temp = 0;
float humidity = 0;

unsigned long lastSend = 0;
const unsigned long SEND_INTERVAL = 60000;


WiFiClient espClient;
PubSubClient client(espClient);


void setup_wifi() {
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}


void startWatering(int seconds) {
  Serial.printf("💧 Starting watering for %d seconds ...\n", seconds);
  //digitalWrite(PUMP_PIN, HIGH);
  delay(seconds * 1000);
  //digitalWrite(PUMP_PIN, LOW);
  Serial.println("Finished watering");
}



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


void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  sensorsSetup();
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(mqttCallback);
  Serial.println(String("Subscribed to topic: ") + (String(CMD_PREFIX)+"/#"));
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastSend >= SEND_INTERVAL) {
    lastSend = now;

    raw[0] = readMoisture(0);
    temp = readTemperature();
    humidity = readHumidity();


    String payload;
    for (int p = 0; p < NUM_MOISTURE_SENSORS; p++) {
      payload += "plant_moisture,user=" + String(USER) +
                ",device=" + String(DEVICE_ID) +
                ",probe=" + String(p + 1) +
                " value=" + String(raw[p]) + "\n";
    }

    payload += "environment,user=" + String(USER) +
              ",device=" + String(DEVICE_ID) +
              " temperature=" + String(temp) +
              ",humidity=" + String(humidity) + "\n";

    client.publish(("telemetry/" + String(USER) + "/" + String(DEVICE_ID)).c_str(),
                  payload.c_str());

  }
}