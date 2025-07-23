#include <Arduino.h>
#include <Wire.h>
#include "netset.h"
//#include <WiFi.h>
//#include <PubSubClient.h>


#include "config.h"
#include "sensors.h"
#include "mqtt.h"
#include "timers.h"
#include "states.h"

//#define MOISTURE_SENSOR1_PIN 32



//float moisture = 0;
//float raw[NUM_MOISTURE_SENSORS];
//float temp = 0;
//float humidity = 0;

//unsigned long lastSend = 0;
//const unsigned long SEND_INTERVAL = 60000;


TaskHandle_t mqttTaskHandle = NULL;


void setup() {
  Serial.begin(115200);
  Wire.begin();
  initStates();
  // Initialize sensors
  sensorsSetup();

  // setup WiFi
  setup_wifi();

  // setup MQTT
  setupMQTT();

  // Create MQTT task pinned to core 1
  xTaskCreatePinnedToCore(mqttTask, "MQTT Task", 4096, NULL, 1, &mqttTaskHandle, 1);

  // start timers
  setupTimers();
  //Serial.println(String("Subscribed to topic: ") + (String(CMD_PREFIX)+"/#"));
}


void loop() {


}