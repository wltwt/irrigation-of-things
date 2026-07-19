#include "services/console.hpp"
#include "app/app.hpp"

#include <Arduino.h>
#include <WiFi.h>

namespace {

WiFiServer server(2323);
WiFiClient client;
String input;

void run_command(const String& command)
{
    if (command == "help") {
        console_println("Commands: help, status, reboot, start, stop");

    } else if (command == "status") {
        client.print("WiFi: ");
        client.println(
            WiFi.status() == WL_CONNECTED
                ? "connected"
                : "disconnected"
        );

        if (WiFi.status() == WL_CONNECTED) {
            client.print("IP: ");
            client.println(WiFi.localIP());
        }

    } else if (command == "reboot") {
        client.println("Rebooting...");
        client.flush();
        delay(100);
        ESP.restart();

    } else if (command == "start") {
        if (app_request_command(AppCommand::Start)) {
            client.println("Start command received.");
        } else {
            client.println("Start command failed.");
        }

    } else if (command == "stop") {
        if (app_request_command(AppCommand::Stop)) {
            client.println("Stop command received.");
        } else {
            client.println("Stop command failed.");
        }

    } else if (!command.isEmpty()) {
        client.println("Unknown command. Type 'help'.");
    }
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
            input.trim();
            run_command(input);
            input = "";
        } else if (character != '\r') {
            input += character;
        }
    }
}