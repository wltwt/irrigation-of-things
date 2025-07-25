#include "sensors.h"
#include "config.h"
#include "states.h"
#include "watering.h"
#include <Adafruit_SHT31.h>

static Adafruit_SHT31 sht30;

bool waterTooLow = false;


void sensorsSetup() {
  if (!sht30.begin(0x44)) {
    Serial.println("SHT31 not found.");
    while (true) delay(1000);
  }
}

float readMoisture(int sensorIndex) {
  if (sensorIndex < 0 || sensorIndex >= NUM_MOISTURE_SENSORS) {
    Serial.printf("Invalid sensor index: %d\n", sensorIndex);
    return -1.0f;
  }

  int pin = MOISTURE_SENSOR_PINS[sensorIndex];
  int raw = analogRead(pin);
  float voltage = (raw / 4095.0f) * 3.3f;
  float moisture = 100.0f * (dry_voltage - voltage) / (dry_voltage - wet_voltage);
  return constrain(moisture, 0.0f, 100.0f);
}

float readWaterLevel() {
    int adc = analogRead(WATER_SENSOR_PIN);
    float delta_mV = 3300.0 / 4096.0; // 0.805 mV per step
    float voltage = adc * delta_mV;
    float litersPerCan = (voltage + 29.61) / 49.75;
    

    if (!waterTooLow && litersPerCan < (MIN_WATER_LEVEL_THRESHOLD - WATER_LEVEL_HYSTERESE)) {
        Serial.println("Water level critically low! Stopping pump.");
        if (xEventGroupGetBits(systemEventGroup) & WATERING_ACTIVE_BIT) {
            stopWatering();
        }
        waterTooLow = true;
    } else if (waterTooLow && litersPerCan > (MIN_WATER_LEVEL_THRESHOLD + WATER_LEVEL_HYSTERESE)) {
        // recovery condition
        Serial.println("Water level recovered.");
        waterTooLow = false;
    }

    float totalLiters = litersPerCan * numCans;

    return totalLiters;
}


float readTemperature() {
  float t = sht30.readTemperature();
  if (isnan(t)) {
    Serial.println("Failed to read temperature");
    return -999.0f;
  }
  return t;
}

float readHumidity() {

  float h = sht30.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read humidity");
    return -999.0f;
  }
  return h;
}
