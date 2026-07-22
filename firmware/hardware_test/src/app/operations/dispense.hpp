#pragma once

enum class DispenseResult
{
    Running,
    Completed,
    Failed
};

bool dispense_start(float target_liters);
DispenseResult dispense_update();
void dispense_cancel();

const char* dispense_state_name();
