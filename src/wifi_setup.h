#pragma once
#include <Arduino.h>

// Connect to the WiFi network defined in credentials.h. Blocks until
// connected or until WIFI_TIMEOUT_MS elapses. Returns true on success.
bool wifiConnect();

// True while the station is associated with the access point.
bool wifiIsConnected();

// Current IP address as a string (empty-ish when not connected).
String wifiIP();
