#pragma once

void sensorsSetup();                        // Init SHT30
float readMoisture(int sensorIndex);        // Returns 0–100%
float readWaterLevel();                     // Returns water level in liters
float readTemperature();                    // Celsius
float readHumidity();                       // RH%
