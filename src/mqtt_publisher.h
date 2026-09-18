#pragma once
#include "bmp280_sensor.h"

// Configure the MQTT client with the broker from credentials.h.
void mqttInit();

// Attempt a (rate-limited, non-blocking) reconnect if disconnected.
// Returns true when connected. Call this every loop iteration.
bool mqttEnsureConnected();

// Service the MQTT client (keep-alive, incoming packets). Call every loop.
void mqttLoop();

// Publish a reading as JSON to MQTT_TOPIC. Returns false if not connected
// or the reading is invalid.
bool mqttPublish(const SensorReading& reading);
