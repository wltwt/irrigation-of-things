#include "sensors.h"
#include "config.h"
#include <Adafruit_SHT31.h>

static Adafruit_SHT31 sht30;

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
  return analogRead(WATER_SENSOR_PIN); 
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
