# Separation of Services

## Telegraf
- Handles automatic data ingestion from MQTT topics
- Pushes data to InfluxDB
- Independent of the web server

## FastAPI
- Handles user authentication and API access
- Sends MQTT commands to devices (e.g. ESP32)
- Does not write sensor data to InfluxDB
