#include "services/ota.hpp"

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <WiFi.h>

#include "config.hpp"

namespace {

bool started = false;

void start_if_connected()
{
    if (!started && WiFi.status() == WL_CONNECTED) {
        ArduinoOTA.begin();
        started = true;
        Serial.println("OTA started");
    }
}

}  // namespace

void ota_init()
{
    ArduinoOTA.setHostname(OTA_HOSTNAME);
    start_if_connected();
}

void ota_update()
{
    start_if_connected();

    if (started) {
        ArduinoOTA.handle();
    }
}
