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

    SensorConfig reservoir;
    SensorConfig moisture;
};

inline constexpr BoardConfig board {
    // relay 1
    .valve = {32, false},
    
    // relay 2
    .pump = {25, true},

    .reservoir = {34},
    .moisture = {33},
};
