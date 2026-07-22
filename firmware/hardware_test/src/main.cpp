#include <Arduino.h>
#include <esp_system.h>
#include <WiFi.h>

#include "app/operations/dispense.hpp"
#include "services/console.hpp"
#include "services/ota.hpp"
#include "services/wifi.hpp"
#include "app/app.hpp"
#include "hardware/flow_sensor.hpp"
#include "hardware/valve.hpp"

namespace {

constexpr uint32_t heartbeat_interval_ms = 5000;

}  // namespace

void setup()
{
    Serial.begin(115200);
    Serial.printf(
        "Startup reached; reset reason: %d\n",
        static_cast<int>(esp_reset_reason()));

    wifi_init();
    Serial.println("WiFi initialized; connection continues in main loop");
    ota_init();
    Serial.println("OTA service initialized");
    console_init();
    Serial.println("Console initialized");

    valve_init();
    Serial.println("Valve initialized");
    flow_sensor_init();
    Serial.println("Flow sensor initialized");
    //app_init();
}

void loop()
{
    wifi_update();
    ota_update();
    console_update();
    app_update();

    static uint32_t last_heartbeat_ms = 0;
    const uint32_t now = millis();

    if (now - last_heartbeat_ms >= heartbeat_interval_ms) {
        last_heartbeat_ms = now;
        Serial.printf(
            "Heartbeat: uptime=%lu ms, wifi=%d, app=%s, operation=%s, "
            "dispense=%s, pulses=%lu\n",
            static_cast<unsigned long>(now),
            static_cast<int>(WiFi.status()),
            app_state_name(app_get_state()),
            operation_name(app_get_operation()),
            dispense_state_name(),
            static_cast<unsigned long>(flow_sensor_pulse_count()));
    }
}
