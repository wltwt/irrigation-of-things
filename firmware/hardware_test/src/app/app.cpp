#include "app.hpp"
#include "logging/log.hpp"
#include "hardware/water_level.hpp"

#include <Arduino.h>

namespace {

enum class AppState
{
    Initializing,
    Idle,
    Running,
    Error
};

AppState state = AppState::Initializing;
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
        case AppState::Running:      return "Running";
        case AppState::Error:        return "Error";
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

        case AppState::Running:
            log_info("Entering Running");
            break;

        case AppState::Error:
            log_info("Entering Error");
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

        case AppState::Running:
            log_info("Leaving Running");
            break;

        case AppState::Error:
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

        case AppState::Running:
            break;

        case AppState::Error:
            break;
    }
}

void process_command(AppCommand command)
{
    switch (command) {
        case AppCommand::Start:
            if (state == AppState::Idle && hardware_ready()) {
                change_state(AppState::Running);
            } else {
                log_info("Start rejected");
            }
            break;

        case AppCommand::Stop:
            if (state == AppState::Running) {
                change_state(AppState::Idle);
            } else {
                log_info("Stop rejected");
            }
            break;

        case AppCommand::Reset:
            if (state != AppState::Running) {
                change_state(AppState::Initializing);
            } else {
                log_info("Reset rejected");
            }
            break;

        case AppCommand::None:
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

void app_update()
{
    if (pending_command != AppCommand::None) {
        const AppCommand command = pending_command;
        pending_command = AppCommand::None;

        process_command(command);
    }

    
    water_level_update();
    update_current_state();
}