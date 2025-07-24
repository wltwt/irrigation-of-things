#include "watering.h"
#include "states.h"
#include "timers.h"
#include "config.h"
#include <Arduino.h>

void startWatering(int seconds) {
    if (xEventGroupGetBits(systemEventGroup) & WATERING_ACTIVE_BIT) {
        Serial.println("Pump already running");
        return;
    }
    Serial.printf("Watering for %d seconds...\n", seconds);
    //digitalWrite(PUMP_PIN, HIGH);
    xEventGroupSetBits(systemEventGroup, WATERING_ACTIVE_BIT);

    xTimerChangePeriod(timerWaterLevel, pdMS_TO_TICKS(SEND_INTERVAL_WATER_FAST), 0);
    xTimerChangePeriod(wateringStopTimer, pdMS_TO_TICKS(seconds * 1000), 0);
    xTimerStart(wateringStopTimer, 0);
}

void stopWatering() {
    //digitalWrite(PUMP_PIN, LOW);
    xEventGroupClearBits(systemEventGroup, WATERING_ACTIVE_BIT);
    xTimerChangePeriod(timerWaterLevel, pdMS_TO_TICKS(SEND_INTERVAL_WATER), 0);
    Serial.println("Watering stopped");
}