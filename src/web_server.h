#pragma once
#include "sensor.h"

// Register routes and start the HTTP server on WEB_SERVER_PORT.
void webServerInit();

// Service pending HTTP clients. Call every loop iteration.
void webServerHandle();

// Update the cached reading served at "/" and "/json".
void webServerUpdateReading(const SensorReading& reading);
