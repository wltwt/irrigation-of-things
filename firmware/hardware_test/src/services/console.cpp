#include "services/console.hpp"

#include "app/app.hpp"
#include "hardware/valve.hpp"
#include "hardware/water_level.hpp"
#include "app/reservoir.hpp"

#include <Arduino.h>
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
void command_valve_stop();
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
    {"valve stop",  command_valve_stop},
    {"water level", command_water_level},
    {"reservoir",   command_reservoir},
};

void command_help()
{
    console_println(
        "Commands: help, status, reboot, start, stop, "
        "valve open, valve close, valve stop"
    );
}

void command_status()
{
    const bool connected = WiFi.status() == WL_CONNECTED;

    client.print("WiFi: ");
    client.println(connected ? "connected" : "disconnected");

    if (connected) {
        client.print("IP: ");
        client.println(WiFi.localIP());
    }

    const bool valve_is_open = valve_status();

    client.println(
        valve_is_open
            ? "valve: open"
            : "valve: closed"
    );
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
    valve_open();
    client.println("Valve opening.");
}

void command_valve_close()
{
    valve_close();
    client.println("Valve closing.");
}

void command_valve_stop()
{
    valve_stop();
    client.println("Valve stopped.");
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
    reservoir_update();
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