#pragma once

enum class AppState
{
    Initializing,
    Idle,
    Busy,
    Fault
};

enum class Operation
{
    None,
    Dispense,
    Irrigation,
    Calibration
};

enum class AppCommand
{
    None,
    Start,
    Stop,
    Reset,
    EnterMotorTest
};


void app_update();

bool app_request_command(AppCommand command);
AppState app_get_state();
Operation app_get_operation();

const char* app_state_name(AppState state);
const char* operation_name(Operation operation);
