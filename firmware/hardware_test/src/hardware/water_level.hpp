#pragma once
#include <arduino.h>


struct WaterLevelReading {
    uint16_t raw;
    float voltage;
    float normalized;
};

void water_level_update();
WaterLevelReading water_level_get_reading();