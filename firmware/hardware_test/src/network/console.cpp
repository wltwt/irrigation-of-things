#include "network/console.hpp"

#include <Arduino.h>
#include <WiFi.h>

namespace {
WiFiServer server(2323);
WiFiClient client;
String input;

void run_command(const String& command)
{
    if (command == "help") {
        client.println("Commands: help, status, reboot, test if it works");
    } else if (command == "status") {
        client.print("WiFi: ");
        client.println(WiFi.status() == WL_CONNECTED ? "connected" : "disconnected");
        if (WiFi.status() == WL_CONNECTED) {
            client.print("IP: ");
            client.println(WiFi.localIP());
        }
    } else if (command == "reboot") {
        client.println("Rebooting...");
        client.flush();
        delay(100);
        ESP.restart();
    } else if (!command.isEmpty()) {
        client.println("Unknown command. Type 'help'.");
    }
}
}  // namespace

void console_init()
{
    server.begin();
}

void console_update()
{
    static bool previously_connected = false;

    const bool connected = client && client.connected();

    if (previously_connected && !connected) {
        Serial.println("Console client disconnected");
    }

    if (!connected) {
        WiFiClient new_client = server.available();

        if (new_client) {
            client = new_client;
            input = "";
            Serial.println("Console client connected");
        }
    }

    previously_connected = client && client.connected();

    while (client && client.available()) {
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
