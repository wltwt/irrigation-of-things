#include "services/wifi.hpp"

#include <Arduino.h>
#include <WiFi.h>

#include "config.hpp"

namespace {
unsigned long last_connection_attempt = 0;

void connect()
{
    Serial.print("Connecting to WiFi");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    last_connection_attempt = millis();
}
}  // namespace

void wifi_init()
{
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);

    connect();

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print('.');
    }

    Serial.print("\nConnected, IP address: ");
    Serial.println(WiFi.localIP());
}

void wifi_update()
{
    static wl_status_t previous_status = WiFi.status();
    const wl_status_t current_status = WiFi.status();

    if (current_status != previous_status) {
        Serial.print("WiFi status changed: ");
        Serial.print(previous_status);
        Serial.print(" -> ");
        Serial.println(current_status);

        if (current_status == WL_CONNECTED) {
            Serial.print("WiFi connected, IP: ");
            Serial.println(WiFi.localIP());
        }

        previous_status = current_status;
    }

    if (current_status == WL_CONNECTED) {
        return;
    }

    if (millis() - last_connection_attempt >= 10000) {
        Serial.println("Retrying WiFi connection");
        WiFi.disconnect();
        connect();
    }
}
