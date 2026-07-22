#include "water_level.hpp"
#include "board/board_config.hpp"


namespace {

constexpr float adc_max = 4095.0f;
constexpr float reference_voltage = 3.3f;
constexpr uint16_t samples_per_average = 16;

uint32_t accumulator = 0;
uint16_t sample_count = 0;
bool initialized = false;

WaterLevelReading current_reading;

}

void water_level_update()
{
    const uint16_t raw = analogRead(board.water_level_sensor.adc_pin);

    accumulator += raw;
    sample_count++;

    float filtered_adc = current_reading.normalized * adc_max;

    if (!initialized) {
        filtered_adc =
            static_cast<float>(accumulator) / sample_count;
    }

    if (sample_count >= samples_per_average) {
        filtered_adc =
            static_cast<float>(accumulator) / sample_count;

        accumulator = 0;
        sample_count = 0;
        initialized = true;
    }

    current_reading = {
        .raw = raw,
        .voltage = filtered_adc / adc_max * reference_voltage,
        .normalized = filtered_adc / adc_max,
    };
}


WaterLevelReading water_level_get_reading()
{
    return current_reading;
}