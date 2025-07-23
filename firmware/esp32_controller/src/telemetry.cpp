#include "telemetry.h"
#include "config.h"
#include "sensors.h"
#include "mqtt.h"



void publishMoisture() {
    String payload;
    for (int p = 0; p < NUM_MOISTURE_SENSORS; p++) {
        float moisture = readMoisture(p);
        payload += "plant_moisture,user=" + String(USER) +
                  ",device=" + String(DEVICE_ID) +
                  ",probe=" + String(p + 1) +
                  " value=" + String(moisture) + "\n";
    }

    client.publish(("telemetry/" + String(USER) + "/" + String(DEVICE_ID)).c_str(),
                   payload.c_str());
}

void publishEnvironment() {
    float temp = readTemperature();
    float humidity = readHumidity();

    String payload = "environment,user=" + String(USER) +
                     ",device=" + String(DEVICE_ID) +
                     " temperature=" + String(temp) +
                     ",humidity=" + String(humidity) + "\n";

    client.publish(("telemetry/" + String(USER) + "/" + String(DEVICE_ID)).c_str(),
                   payload.c_str());
}

void publishWaterLevel() {
    float level = readWaterLevel();

    String payload = "water_level,user=" + String(USER) +
                     ",device=" + String(DEVICE_ID) +
                     " value=" + String(level) + "\n";

    client.publish(("telemetry/" + String(USER) + "/" + String(DEVICE_ID)).c_str(),
                   payload.c_str());
}
