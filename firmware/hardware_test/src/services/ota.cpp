#include "services/ota.hpp"

#include <ArduinoOTA.h>

#include "config.hpp"

void ota_init()
{
    ArduinoOTA.setHostname(OTA_HOSTNAME);
    ArduinoOTA.begin();
}

void ota_update()
{
    ArduinoOTA.handle();
}
