// config.h
#pragma once

#include "secrets.h"

// Wifi-config
//constexpr char* ssid = "your_ssid";
//constexpr char* password = "your_password";
//constexpr char* mqtt_server = "your_mqtt_server";

// Moisture limits
constexpr float dry_voltage = 3.0;
constexpr float wet_voltage = 0.87;

// MQTT details
//constexpr const char* DEVICE_ID  = "your_device_id";
//constexpr const char* USER       = "your_username";
//constexpr const char* CMD_PREFIX = "your_cmd_prefix";

// Number of moisture probes
constexpr int probes = 2;

// Pins
constexpr int PUMP_PIN = 26;
constexpr int WATER_SENSOR_PIN = 34;
constexpr int MOISTURE_SENSOR_PINS[] = {32};
constexpr int NUM_MOISTURE_SENSORS = sizeof(MOISTURE_SENSOR_PINS) / sizeof(int);

// Sensor cutoff values
constexpr int WATER_CUTOFF = 100;

// Timer intervals in milliseconds
//constexpr int SEND_INTERVAL_MOISTURE = 60000; // 1 minute
//constexpr int SEND_INTERVAL_ENV = 60000;      // 1 minute
//constexpr int SEND_INTERVAL_WATER = 60000;    // 1 minute

// Timers
#define SEND_INTERVAL_MOISTURE  (10 * 60 * 1000UL)   // 10m
#define SEND_INTERVAL_ENV       (10 * 60 * 1000UL)   // 10m
#define SEND_INTERVAL_WATER     (60 * 60 * 1000UL)   // 1h
#define SEND_INTERVAL_WATER_FAST (5 * 1000UL)        // 5s
