#pragma once

struct ReservoirState {
    float liters;
    float percent_full;
    bool low_water;
    bool valid;
};

void reservoir_init();
void reservoir_update();

ReservoirState reservoir_get_state();