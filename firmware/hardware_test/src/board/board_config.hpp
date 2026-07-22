#pragma once

struct RelayConfig {
    int pin;
    bool active_low;
};

struct AnalogSensorConfig {
    int adc_pin;
};

struct FlowSensorConfig {
    int pulse_pin;
};

struct BoardConfig {
    RelayConfig valve;
    RelayConfig pump;

    AnalogSensorConfig water_level_sensor;
    AnalogSensorConfig moisture;
    FlowSensorConfig flow_sensor;
};

inline constexpr BoardConfig board {
    // relay 1 pin
    .valve = {32, false},
    
    // relay 2 pin
    .pump = {25, true},

    // water level sensor pin
    .water_level_sensor = {34},
    .moisture = {33},
    .flow_sensor = {35},
};
