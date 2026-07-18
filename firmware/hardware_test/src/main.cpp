#include <Arduino.h>

#include "network/console.hpp"
#include "network/ota.hpp"
#include "network/wifi.hpp"

void setup()
{
    Serial.begin(115200);

    wifi_init();
    ota_init();
    console_init();
}

void loop()
{
    wifi_update();
    ota_update();
    console_update();
}
