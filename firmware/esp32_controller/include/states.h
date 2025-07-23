#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>

enum WateringState { IDLE, WATERING };

extern volatile WateringState wateringState;
extern EventGroupHandle_t systemEventGroup;

#define WIFI_CONNECTED_BIT   (1 << 0)
#define MQTT_CONNECTED_BIT   (1 << 1)
#define WATERING_ACTIVE_BIT  (1 << 2)


void initStates();