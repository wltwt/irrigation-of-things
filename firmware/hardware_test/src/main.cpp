#include <Arduino.h>

#include "services/console.hpp"
#include "services/ota.hpp"
#include "services/wifi.hpp"
#include "app/app.hpp"

void setup()
{
    Serial.begin(115200);

    wifi_init();
    ota_init();
    console_init();
    //app_init();
}

void loop()
{
    wifi_update();
    ota_update();
    console_update();
    app_update();
}
