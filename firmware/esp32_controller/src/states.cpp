#include "states.h"

volatile WateringState wateringState = IDLE;
EventGroupHandle_t systemEventGroup = nullptr;

void initStates() {
    systemEventGroup = xEventGroupCreate();
}
