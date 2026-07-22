#include "flow_sensor.hpp"

#include "board/board_config.hpp"

#include <Arduino.h>

namespace {

// Provisional empirical calibration from three physical tests measuring
// approximately 440-448 pulses for about 0.9 liters.
constexpr float pulses_per_liter = 495.0f;

volatile uint32_t pulse_count = 0;
portMUX_TYPE pulse_count_mutex = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR handle_pulse()
{
    portENTER_CRITICAL_ISR(&pulse_count_mutex);
    pulse_count++;
    portEXIT_CRITICAL_ISR(&pulse_count_mutex);
}

}  // namespace

void flow_sensor_init()
{
    flow_sensor_reset();
    pinMode(board.flow_sensor.pulse_pin, INPUT);
    attachInterrupt(
        digitalPinToInterrupt(board.flow_sensor.pulse_pin),
        handle_pulse,
        RISING);
}

void flow_sensor_reset()
{
    portENTER_CRITICAL(&pulse_count_mutex);
    pulse_count = 0;
    portEXIT_CRITICAL(&pulse_count_mutex);
}

uint32_t flow_sensor_pulse_count()
{
    portENTER_CRITICAL(&pulse_count_mutex);
    const uint32_t count = pulse_count;
    portEXIT_CRITICAL(&pulse_count_mutex);
    return count;
}

float flow_sensor_liters()
{
    return flow_sensor_liters(flow_sensor_pulse_count());
}

float flow_sensor_liters(uint32_t count)
{
    return static_cast<float>(count) / pulses_per_liter;
}
