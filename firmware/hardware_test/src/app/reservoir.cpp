#include "reservoir.hpp"

#include "hardware/water_level.hpp"

#include <algorithm>

namespace {

constexpr float reservoir_capacity_liters = 20.0f;
constexpr float low_water_threshold_liters = 2.0f;

// Temporary calibration data
//
// 4 L  -> raw 115
// 10 L -> raw 459
// 15 L -> raw 742
// 20 L -> raw ca. 1030
//
// about 57.5 ADC-value per liter.

constexpr float calibration_raw_at_4_liters = 115.0f;
constexpr float calibration_liters_at_reference = 4.0f;
constexpr float calibration_raw_per_liter = 57.5f;

ReservoirState current_state {};

float raw_to_liters(uint16_t raw)
{
    const float raw_value = static_cast<float>(raw);

    const float liters =
        calibration_liters_at_reference +
        (raw_value - calibration_raw_at_4_liters) /
        calibration_raw_per_liter;

    return std::clamp(
        liters,
        0.0f,
        reservoir_capacity_liters
    );
}

float liters_to_percent(float liters)
{
    return std::clamp(
        liters / reservoir_capacity_liters,
        0.0f,
        1.0f
    );
}

} // namespace

void reservoir_init()
{
    current_state = {};
}

void reservoir_update()
{
    const WaterLevelReading reading =
        water_level_get_reading();

    current_state.liters = raw_to_liters(reading.raw);

    current_state.percent_full =
        liters_to_percent(current_state.liters);

    current_state.low_water =
        current_state.liters <= low_water_threshold_liters;
}

ReservoirState reservoir_get_state()
{
    return current_state;
}