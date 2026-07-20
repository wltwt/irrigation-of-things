#pragma once

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