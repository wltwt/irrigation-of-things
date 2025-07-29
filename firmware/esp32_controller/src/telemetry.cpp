#include "telemetry.h"
#include "config.h"
#include "sensors.h"
#include "mqtt.h"



void publishMoisture() {
    char topic[64];
    snprintf(topic, sizeof(topic), "telemetry/%s/%s", USER, DEVICE_ID);

    char payload[128];
    size_t offset = 0;
    for (int p = 0; p < NUM_MOISTURE_SENSORS; p++) {
        float moisture = readMoisture(p);
        int written = snprintf(payload + offset, sizeof(payload) - offset,
                               "plant_moisture,user=%s,device=%s,probe=%d value=%.2f\n",
                               USER, DEVICE_ID, p + 1, moisture);
        if (written < 0 || (size_t)written >= sizeof(payload) - offset) break;
        offset += written;
    }
    client.publish(topic, payload);
}

void publishEnvironment() {
    float temp = readTemperature();
    float humidity = readHumidity();

    char topic[64];
    snprintf(topic, sizeof(topic), "telemetry/%s/%s", USER, DEVICE_ID);

    char payload[128];
    // Include free heap memory in telemetry for monitoring
    size_t freeHeap = ESP.getFreeHeap();
    snprintf(payload, sizeof(payload),
             "environment,user=%s,device=%s temperature=%.2f,humidity=%.2f,free_heap=%u\n",
             USER, DEVICE_ID, temp, humidity, static_cast<unsigned int>(freeHeap));
    client.publish(topic, payload);
}

void publishWaterLevel() {
    float level = readWaterLevel();

    char topic[64];
    snprintf(topic, sizeof(topic), "telemetry/%s/%s", USER, DEVICE_ID);

    char payload[128];
    snprintf(payload, sizeof(payload),
             "water_level,user=%s,device=%s value=%.2f\n",
             USER, DEVICE_ID, level);
    client.publish(topic, payload);
}
