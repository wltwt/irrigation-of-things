#pragma once

#include <cstdint>

void flow_sensor_init();
void flow_sensor_reset();

uint32_t flow_sensor_pulse_count();
float flow_sensor_liters();
float flow_sensor_liters(uint32_t pulse_count);
