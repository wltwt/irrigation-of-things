#pragma once
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>


extern TimerHandle_t timerMoisture;
extern TimerHandle_t timerEnvironment;
extern TimerHandle_t timerWaterLevel;
extern TimerHandle_t wateringStopTimer;


void setupTimers();
void cbMoisture(TimerHandle_t xTimer);
void cbEnvironment(TimerHandle_t xTimer);
void cbWaterLevel(TimerHandle_t xTimer);
void cbStopWatering(TimerHandle_t xTimer);