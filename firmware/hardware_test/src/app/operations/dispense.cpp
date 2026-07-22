#include "dispense.hpp"

#include "app/reservoir.hpp"
#include "hardware/flow_sensor.hpp"
#include "hardware/valve.hpp"
#include "logging/log.hpp"

#include <Arduino.h>
#include <cmath>

namespace {

constexpr uint32_t valve_opening_time_ms = 20000;
constexpr uint32_t maximum_dispensing_time_ms = 120000;
constexpr uint32_t valve_closing_time_ms = 10000;
constexpr uint32_t settling_time_ms = 8000;
constexpr uint32_t log_interval_ms = 1000;
constexpr float flow_stop_margin_liters = 0.52f;
constexpr float manually_measured_test_volume_liters = 0.0f;

enum class DispenseState
{
    Idle,
    Opening,
    Dispensing,
    Closing,
    Settling,
    Completed,
    Failed
};

DispenseState state = DispenseState::Idle;
float starting_liters = 0.0f;
float target_liters = 0.0f;
float flow_at_closing_liters = 0.0f;
uint32_t opening_started_ms = 0;
uint32_t dispensing_started_ms = 0;
uint32_t closing_started_ms = 0;
uint32_t settling_started_ms = 0;
uint32_t last_log_ms = 0;

bool is_active()
{
    return state == DispenseState::Opening ||
           state == DispenseState::Dispensing ||
           state == DispenseState::Closing ||
           state == DispenseState::Settling;
}

DispenseResult fail_safely(const char* message)
{
    log_info(message);
    valve_close();
    state = DispenseState::Failed;
    return DispenseResult::Failed;
}

}  // namespace

bool dispense_start(float requested_target_liters)
{
    if (is_active() || requested_target_liters <= 0.0f) {
        return false;
    }

    const ReservoirState reservoir = reservoir_get_state();

    if (!std::isfinite(reservoir.liters) ||
        reservoir.liters <= 0.0f ||
        reservoir.liters < requested_target_liters) {
        return false;
    }

    starting_liters = reservoir.liters;
    target_liters = requested_target_liters;
    flow_at_closing_liters = 0.0f;
    opening_started_ms = millis();
    dispensing_started_ms = 0;
    closing_started_ms = 0;
    settling_started_ms = 0;
    last_log_ms = opening_started_ms;
    flow_sensor_reset();

    const float closing_threshold_liters =
        target_liters > flow_stop_margin_liters
            ? target_liters - flow_stop_margin_liters
            : 0.0f;

    char buffer[176];
    snprintf(
        buffer,
        sizeof(buffer),
        "Dispense baseline:\n"
        "Reservoir: %.2f L\n"
        "Target: %.2f L\n"
        "Stop margin: %.2f L\n"
        "Closing threshold: %.2f L",
        starting_liters,
        target_liters,
        flow_stop_margin_liters,
        closing_threshold_liters);
    log_info(buffer);

    if (closing_threshold_liters == 0.0f) {
        log_info(
            "Closing threshold is 0.00 L; Closing will begin immediately "
            "after the valve-opening phase");
    }

    valve_open();
    state = DispenseState::Opening;
    return true;
}

DispenseResult dispense_update()
{
    switch (state) {
        case DispenseState::Idle:
            return DispenseResult::Failed;

        case DispenseState::Opening: {
            const uint32_t now = millis();
            const uint32_t opening_elapsed_ms =
                now - opening_started_ms;
            const float current_liters = reservoir_get_state().liters;

            if (!std::isfinite(current_liters)) {
                return fail_safely(
                    "Dispense failed: unusable reservoir reading");
            }

            const float dispensed_liters = starting_liters - current_liters;
            const uint32_t pulses = flow_sensor_pulse_count();
            const float flow_liters = flow_sensor_liters();

            if (now - last_log_ms >= log_interval_ms) {
                char buffer[176];
                snprintf(
                    buffer,
                    sizeof(buffer),
                    "Dispense opening: Reservoir %.2f L | Dispensed %.2f L "
                    "| Flow %.2f L | Pulses %lu | elapsed %lu ms",
                    current_liters,
                    dispensed_liters,
                    flow_liters,
                    static_cast<unsigned long>(pulses),
                    static_cast<unsigned long>(opening_elapsed_ms));
                log_info(buffer);
                last_log_ms = now;
            }

            if (opening_elapsed_ms >= valve_opening_time_ms) {
                log_info(
                    "Valve opening travel completed; entering "
                    "flow-controlled dispensing");
                // Keep the relay ON. The valve's internal end stop has stopped
                // the motor, and the asserted relay keeps it commanded open.
                dispensing_started_ms = now;
                last_log_ms = now;
                state = DispenseState::Dispensing;
            }
            return DispenseResult::Running;
        }

        case DispenseState::Dispensing: {
            const uint32_t now = millis();
            const float current_liters = reservoir_get_state().liters;

            if (!std::isfinite(current_liters)) {
                return fail_safely(
                    "Dispense failed: unusable reservoir reading");
            }

            const float dispensed_liters = starting_liters - current_liters;
            const uint32_t pulses = flow_sensor_pulse_count();
            const float flow_liters = flow_sensor_liters();
            const uint32_t dispensing_elapsed_ms =
                now - dispensing_started_ms;

            if (now - last_log_ms >= log_interval_ms) {
                char buffer[176];
                snprintf(
                    buffer,
                    sizeof(buffer),
                    "Dispense dispensing: Reservoir %.2f L | Flow %.2f L "
                    "| Pulses %lu | elapsed %lu ms",
                    current_liters,
                    flow_liters,
                    static_cast<unsigned long>(pulses),
                    static_cast<unsigned long>(dispensing_elapsed_ms));
                log_info(buffer);
                last_log_ms = now;
            }

            const float stop_at_liters =
                target_liters > flow_stop_margin_liters
                    ? target_liters - flow_stop_margin_liters
                    : 0.0f;
            const bool threshold_reached = flow_liters >= stop_at_liters;
            const bool dispensing_timed_out =
                dispensing_elapsed_ms >= maximum_dispensing_time_ms;

            if (threshold_reached || dispensing_timed_out) {
                char stop_reason[144];

                if (threshold_reached) {
                    snprintf(
                        stop_reason,
                        sizeof(stop_reason),
                        "Dispense threshold reached:\n"
                        "target %.2f L\n"
                        "margin %.2f L\n"
                        "threshold %.2f L\n"
                        "measured %.2f L\n"
                        "pulses %lu",
                        target_liters,
                        flow_stop_margin_liters,
                        stop_at_liters,
                        flow_liters,
                        static_cast<unsigned long>(pulses));
                } else {
                    snprintf(
                        stop_reason,
                        sizeof(stop_reason),
                        "Dispense timeout: target %.2f L, measured %.2f L, "
                        "pulses %lu",
                        target_liters,
                        flow_liters,
                        static_cast<unsigned long>(pulses));
                }
                log_info(stop_reason);

                flow_at_closing_liters = flow_liters;
                valve_close();
                log_info("Dispense entering Closing");
                closing_started_ms = now;
                last_log_ms = now;
                state = DispenseState::Closing;
            }

            return DispenseResult::Running;
        }

        case DispenseState::Closing: {
            const uint32_t now = millis();
            const float current_liters = reservoir_get_state().liters;

            if (!std::isfinite(current_liters)) {
                return fail_safely(
                    "Dispense failed: unusable reservoir reading");
            }

            const float dispensed_liters = starting_liters - current_liters;
            const uint32_t closing_elapsed_ms =
                now - closing_started_ms;

            if (now - last_log_ms >= log_interval_ms) {
                const uint32_t pulses = flow_sensor_pulse_count();
                const float flow_liters = flow_sensor_liters();
                char buffer[176];
                snprintf(
                    buffer,
                    sizeof(buffer),
                    "Dispense closing: Reservoir %.2f L | Dispensed %.2f L "
                    "| Flow %.2f L | Pulses %lu | elapsed %lu ms",
                    current_liters,
                    dispensed_liters,
                    flow_liters,
                    static_cast<unsigned long>(pulses),
                    static_cast<unsigned long>(closing_elapsed_ms));
                log_info(buffer);
                last_log_ms = now;
            }

            if (closing_elapsed_ms >= valve_closing_time_ms) {
                log_info(
                    "Valve closing travel completed; entering Settling");
                settling_started_ms = now;
                last_log_ms = now;
                state = DispenseState::Settling;
            }
            return DispenseResult::Running;
        }

        case DispenseState::Settling: {
            const uint32_t now = millis();
            const float current_liters = reservoir_get_state().liters;

            if (!std::isfinite(current_liters)) {
                return fail_safely(
                    "Dispense failed: unusable reservoir reading");
            }

            const float dispensed_liters = starting_liters - current_liters;
            const uint32_t settling_elapsed_ms =
                now - settling_started_ms;

            if (settling_elapsed_ms >= settling_time_ms) {
                const float final_error =
                    dispensed_liters - target_liters;
                const uint32_t pulses = flow_sensor_pulse_count();
                const float flow_liters = flow_sensor_liters();
                const float measurement_difference =
                    dispensed_liters - flow_liters;
                const float closing_overrun_liters =
                    flow_liters - flow_at_closing_liters;
                const float flow_final_error_liters =
                    flow_liters - target_liters;

                char buffer[160];
                snprintf(
                    buffer,
                    sizeof(buffer),
                    "Dispense completed: target %.2f L, dispensed %.2f L, "
                    "error %+.2f L, reservoir %.2f L",
                    target_liters,
                    dispensed_liters,
                    final_error,
                    current_liters);
                log_info(buffer);

                char summary[176];
                snprintf(
                    summary,
                    sizeof(summary),
                    "Dispense test summary: pulses %lu, flow %.2f L, "
                    "reservoir estimate %.2f L, "
                    "reservoir-flow difference %+.2f L",
                    static_cast<unsigned long>(pulses),
                    flow_liters,
                    dispensed_liters,
                    measurement_difference);
                log_info(summary);

                char compensation_summary[192];
                snprintf(
                    compensation_summary,
                    sizeof(compensation_summary),
                    "Flow compensation summary:\n"
                    "Requested target: %.2f L\n"
                    "Closing started at: %.2f L\n"
                    "Final flow volume: %.2f L\n"
                    "Closing overrun: %.2f L\n"
                    "Final error: %+.2f L",
                    target_liters,
                    flow_at_closing_liters,
                    flow_liters,
                    closing_overrun_liters,
                    flow_final_error_liters);
                log_info(compensation_summary);

                if (manually_measured_test_volume_liters > 0.0f) {
                    const float measured_pulses_per_liter =
                        static_cast<float>(pulses) /
                        manually_measured_test_volume_liters;

                    char calibration[112];
                    snprintf(
                        calibration,
                        sizeof(calibration),
                        "Manual calibration: %lu pulses / %.3f L = "
                        "%.1f pulses/L",
                        static_cast<unsigned long>(pulses),
                        manually_measured_test_volume_liters,
                        measured_pulses_per_liter);
                    log_info(calibration);
                }

                state = DispenseState::Completed;
                return DispenseResult::Completed;
            }

            if (now - last_log_ms >= log_interval_ms) {
                const uint32_t pulses = flow_sensor_pulse_count();
                const float flow_liters = flow_sensor_liters();
                char buffer[176];
                snprintf(
                    buffer,
                    sizeof(buffer),
                    "Dispense settling: Reservoir %.2f L | Dispensed %.2f L "
                    "| Flow %.2f L | Pulses %lu | elapsed %lu ms",
                    current_liters,
                    dispensed_liters,
                    flow_liters,
                    static_cast<unsigned long>(pulses),
                    static_cast<unsigned long>(settling_elapsed_ms));
                log_info(buffer);
                last_log_ms = now;
            }

            return DispenseResult::Running;
        }

        case DispenseState::Completed:
            return DispenseResult::Completed;

        case DispenseState::Failed:
            return DispenseResult::Failed;
    }

    return DispenseResult::Failed;
}

void dispense_cancel()
{
    valve_close();
    state = DispenseState::Idle;
    starting_liters = 0.0f;
    target_liters = 0.0f;
    flow_at_closing_liters = 0.0f;
    opening_started_ms = 0;
    dispensing_started_ms = 0;
    closing_started_ms = 0;
    settling_started_ms = 0;
    last_log_ms = 0;
}

const char* dispense_state_name()
{
    switch (state) {
        case DispenseState::Idle:       return "Idle";
        case DispenseState::Opening:    return "Opening";
        case DispenseState::Dispensing: return "Dispensing";
        case DispenseState::Closing:    return "Closing";
        case DispenseState::Settling:   return "Settling";
        case DispenseState::Completed:  return "Completed";
        case DispenseState::Failed:     return "Failed";
    }

    return "Unknown";
}
