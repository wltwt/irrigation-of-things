#include "app.hpp"
#include "app/operations/dispense.hpp"
#include "app/reservoir.hpp"
#include "logging/log.hpp"
#include "hardware/water_level.hpp"

#include <Arduino.h>

namespace {

AppState state = AppState::Initializing;
Operation active_operation = Operation::None;
AppCommand pending_command = AppCommand::None;

void process_command(AppCommand command);
void change_state(AppState new_state);
void update_current_state();

void enter_state(AppState new_state);
void exit_state(AppState old_state);

bool hardware_ready()
{
    // Midlertidig placeholder.
    return true;
}

const char* state_to_string(AppState state)
{
    switch (state) {
        case AppState::Initializing: return "Initializing";
        case AppState::Idle:         return "Idle";
        case AppState::Busy:         return "Busy";
        case AppState::Fault:        return "Fault";
    }

    return "Unknown";
}

void change_state(AppState new_state)
{
    if (new_state == state) {
        return;
    }

    const AppState old_state = state;

    exit_state(old_state);
    state = new_state;

    char buffer[64];

    snprintf(
        buffer,
        sizeof(buffer),
        "State: %s -> %s",
        state_to_string(old_state),
        state_to_string(new_state));

    log_info(buffer);
    enter_state(new_state);
}

void enter_state(AppState new_state)
{
    switch (new_state) {
        case AppState::Initializing:
            log_info("Entering Initializing");
            break;

        case AppState::Idle:
            log_info("Entering Idle");
            break;

        case AppState::Busy:
            log_info("Entering Busy");
            break;

        case AppState::Fault:
            log_info("Entering Fault");
            break;
    }
}

void exit_state(AppState old_state)
{
    switch (old_state) {
        case AppState::Initializing:
            break;

        case AppState::Idle:
            break;

        case AppState::Busy:
            log_info("Leaving Busy");
            break;

        case AppState::Fault:
            break;
    }
}

void update_current_state()
{
    switch (state) {
        case AppState::Initializing:
            if (hardware_ready()) {
                change_state(AppState::Idle);
            }
            break;

        case AppState::Idle:
            break;

        case AppState::Busy:
            if (active_operation == Operation::Dispense) {
                const DispenseResult result = dispense_update();

                if (result != DispenseResult::Running) {
                    active_operation = Operation::None;
                    change_state(AppState::Idle);
                }
            }
            break;

        case AppState::Fault:
            break;
    }
}

void process_command(AppCommand command)
{
    switch (command) {
        case AppCommand::Start:
            if (state == AppState::Idle &&
                hardware_ready() &&
                dispense_start(1.0f)) {
                active_operation = Operation::Dispense;
                change_state(AppState::Busy);
            } else {
                log_info("Start rejected");
            }
            break;

        case AppCommand::Stop:
            if (state == AppState::Busy) {
                if (active_operation == Operation::Dispense) {
                    dispense_cancel();
                }
                active_operation = Operation::None;
                change_state(AppState::Idle);
            } else {
                log_info("Stop rejected");
            }
            break;

        case AppCommand::Reset:
            if (state != AppState::Busy) {
                active_operation = Operation::None;
                change_state(AppState::Initializing);
            } else {
                log_info("Reset rejected");
            }
            break;

        case AppCommand::None:
        case AppCommand::EnterMotorTest:
            break;
    }
}

}  // namespace

bool app_request_command(AppCommand command)
{
    if (command == AppCommand::None) {
        return false;
    }

    if (pending_command != AppCommand::None) {
        return false;
    }

    pending_command = command;
    return true;
}

AppState app_get_state()
{
    return state;
}

Operation app_get_operation()
{
    return active_operation;
}

const char* app_state_name(AppState state)
{
    return state_to_string(state);
}

const char* operation_name(Operation operation)
{
    switch (operation) {
        case Operation::None:        return "None";
        case Operation::Dispense:    return "Dispense";
        case Operation::Irrigation:  return "Irrigation";
        case Operation::Calibration: return "Calibration";
    }

    return "Unknown";
}

void app_update()
{
    if (pending_command != AppCommand::None) {
        const AppCommand command = pending_command;
        pending_command = AppCommand::None;

        process_command(command);
    }

    water_level_update();
    reservoir_update();
    update_current_state();
}
