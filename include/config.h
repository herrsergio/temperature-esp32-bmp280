#pragma once

// Non-secret configuration. Secrets (WiFi and MQTT credentials) live in
// credentials.h, which is gitignored. See credentials.h.example.

// Serial
#define SERIAL_BAUD 115200

// BMP280 over I2C.
// Most GY-BMP280 breakout boards use address 0x76; some use 0x77.
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22
#define BMP280_I2C_ADDRESS 0x77

// Reference sea-level pressure in hPa, used to derive altitude.
// Adjust to your local QNH for an accurate altitude value.
#define SEALEVEL_HPA 1013.25f

// How often to read the sensor, print, publish and refresh the web cache (ms).
#define READING_INTERVAL_MS 10000UL

// WiFi connection timeout (ms).
#define WIFI_TIMEOUT_MS 20000UL

// MQTT (non-secret parts; host/port/user/password are in credentials.h).
#define MQTT_TOPIC "esp32/bmp280"
#define MQTT_CLIENT_ID "esp32-bmp280"
#define MQTT_RECONNECT_INTERVAL_MS 5000UL
#define MQTT_RETAIN true

// HTTP server.
#define WEB_SERVER_PORT 80
