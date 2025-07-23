#include <Arduino.h>
#include "timers.h"
#include "watering.h"
#include "config.h"
#include "mqtt.h"
#include "telemetry.h"


TimerHandle_t timerMoisture;
TimerHandle_t timerEnvironment;
TimerHandle_t timerWaterLevel;
TimerHandle_t wateringStopTimer;


void setupTimers() {
    timerMoisture = xTimerCreate(
        "Moisture",
        pdMS_TO_TICKS(SEND_INTERVAL_MOISTURE),
        pdTRUE,
        0,
        cbMoisture
    );

    timerEnvironment = xTimerCreate(
        "Environment",
        pdMS_TO_TICKS(SEND_INTERVAL_ENV),
        pdTRUE,
        0,
        cbEnvironment
    );

    timerWaterLevel = xTimerCreate(
        "WaterLevel",
        pdMS_TO_TICKS(SEND_INTERVAL_WATER),
        pdTRUE,
        0,
        cbWaterLevel
    );

    wateringStopTimer = xTimerCreate(
        "StopWater",
        pdMS_TO_TICKS(5000),
        pdFALSE,
        0,
        cbStopWatering
    );

    // start timers
    xTimerStart(timerMoisture, 0);
    xTimerStart(timerEnvironment, 0);
    xTimerStart(timerWaterLevel, 0);
}

void cbMoisture(TimerHandle_t xTimer) {
    publishMoisture();
}

void cbEnvironment(TimerHandle_t xTimer) {
    publishEnvironment();
}

void cbWaterLevel(TimerHandle_t xTimer) {
    publishWaterLevel();
}

void cbStopWatering(TimerHandle_t xTimer) {
    stopWatering();
}