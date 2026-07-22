#pragma once

struct RelayConfig {
    int pin;
    bool active_low;
};

struct SensorConfig {
    int adc_pin;
};

struct BoardConfig {
    RelayConfig valve;
    RelayConfig pump;

    SensorConfig water_level_sensor;
    SensorConfig moisture;
};

inline constexpr BoardConfig board {
    // relay 1 pin
    .valve = {32, false},
    
    // relay 2 pin
    .pump = {25, true},

    // water level sensor pin
    .water_level_sensor = {34},
    .moisture = {33},
};
