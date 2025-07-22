#pragma once

void sensorsSetup();                        // Init f.eks. SHT30
float readMoisture(int sensorIndex);        // Returnerer 0–100%
float readWaterLevel();                     // Returnerer ADC-verdi
float readTemperature();                    // Celsius
float readHumidity();                       // RH%
