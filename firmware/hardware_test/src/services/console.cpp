#include "services/console.hpp"

#include "app/app.hpp"
#include "app/operations/dispense.hpp"
#include "hardware/flow_sensor.hpp"
#include "hardware/valve.hpp"
#include "hardware/water_level.hpp"
#include "app/reservoir.hpp"

#include <Arduino.h>
#include <esp_system.h>
#include <WiFi.h>

namespace {

WiFiServer server(2323);
WiFiClient client;
String input;

using CommandHandler = void (*)();

struct CommandEntry {
    const char* name;
    CommandHandler handler;
};

void command_help();
void command_status();
void command_reboot();
void command_start();
void command_stop();
void command_valve_open();
void command_valve_close();
void command_water_level();
void command_reservoir();


constexpr CommandEntry commands[] = {
    {"help",        command_help},
    {"status",      command_status},
    {"reboot",      command_reboot},
    {"start",       command_start},
    {"stop",        command_stop},
    {"valve open",  command_valve_open},
    {"valve close", command_valve_close},
    {"water level", command_water_level},
    {"reservoir",   command_reservoir},
};

const char* wifi_status_name(wl_status_t status)
{
    switch (status) {
        case WL_IDLE_STATUS:     return "idle";
        case WL_NO_SSID_AVAIL:   return "SSID unavailable";
        case WL_SCAN_COMPLETED:  return "scan completed";
        case WL_CONNECTED:       return "connected";
        case WL_CONNECT_FAILED:  return "connection failed";
        case WL_CONNECTION_LOST: return "connection lost";
        case WL_DISCONNECTED:    return "disconnected";
        case WL_NO_SHIELD:       return "no Wi-Fi hardware";
    }

    return "unknown";
}

void command_help()
{
    console_println(
        "Commands: help, status, reboot, start, stop, "
        "valve open, valve close"
    );
}

void command_status()
{
    const wl_status_t wifi_status = WiFi.status();
    const uint32_t pulses = flow_sensor_pulse_count();

    client.printf("Uptime: %lu ms\n", static_cast<unsigned long>(millis()));
    client.printf("WiFi: %s\n", wifi_status_name(wifi_status));

    if (wifi_status == WL_CONNECTED) {
        client.print("IP: ");
        client.println(WiFi.localIP());
        client.printf("RSSI: %d dBm\n", WiFi.RSSI());
    }

    client.printf("App state: %s\n", app_state_name(app_get_state()));
    client.printf("Operation: %s\n", operation_name(app_get_operation()));
    client.printf("Dispense state: %s\n", dispense_state_name());
    client.printf("Flow pulses: %lu\n", static_cast<unsigned long>(pulses));
    client.printf("Flow volume: %.3f L\n", flow_sensor_liters(pulses));
    client.printf(
        "Reset reason: %d\n",
        static_cast<int>(esp_reset_reason()));
    client.printf(
        "Free heap: %lu bytes\n",
        static_cast<unsigned long>(ESP.getFreeHeap()));
}

void command_reboot()
{
    client.println("Rebooting...");
    client.flush();

    delay(100);
    ESP.restart();
}

void command_start()
{
    const bool accepted = app_request_command(AppCommand::Start);

    client.println(
        accepted
            ? "Start command received."
            : "Start command failed."
    );
}

void command_stop()
{
    const bool accepted = app_request_command(AppCommand::Stop);

    client.println(
        accepted
            ? "Stop command received."
            : "Stop command failed."
    );
}

void command_valve_open()
{
    if (app_get_state() == AppState::Busy) {
        client.println("Valve command rejected: operation active.");
        return;
    }

    valve_open();
    client.println("Valve opening.");
}

void command_valve_close()
{
    if (app_get_state() == AppState::Busy) {
        client.println("Valve command rejected: operation active.");
        return;
    }

    valve_close();
    client.println("Valve closing.");
}

void command_water_level()
{
    const WaterLevelReading reading =
        water_level_get_reading();

    client.printf("Raw: %u\n", reading.raw);
    client.printf("Voltage: %.2f V\n", reading.voltage);
    client.printf("Normalized: %.3f\n", reading.normalized);
}

void command_reservoir()
{
    const ReservoirState state = reservoir_get_state();

    client.printf("reservoir");
    client.printf("Liters: %.1f L\n", state.liters);
    client.printf("Full: %.1f %%\n", state.percent_full * 100.0f);
    client.printf(
        "Low water: %s\n",
        state.low_water ? "yes" : "no"
    );
}

void run_command(String command)
{
    command.trim();
    command.toLowerCase();

    if (command.isEmpty()) {
        return;
    }

    for (const auto& entry : commands) {
        if (command == entry.name) {
            entry.handler();
            return;
        }
    }

    client.println("Unknown command. Type 'help'.");
}

}  // namespace

void console_init()
{
    server.begin();
}

bool console_connected()
{
    return client && client.connected();
}

void console_println(const char* text)
{
    if (console_connected()) {
        client.println(text);
    }
}

void console_update()
{
    static bool previously_connected = false;

    if (previously_connected && !console_connected()) {
        Serial.println("Console client disconnected");
    }

    if (!console_connected()) {
        WiFiClient new_client = server.available();

        if (new_client) {
            client = new_client;
            input = "";
            Serial.println("Console client connected");
        }
    }

    previously_connected = console_connected();

    while (console_connected() && client.available()) {
        const char character = static_cast<char>(client.read());

        if (character == '\n') {
            run_command(input);
            input = "";
        } else if (character != '\r') {
            input += character;
        }
    }
}
