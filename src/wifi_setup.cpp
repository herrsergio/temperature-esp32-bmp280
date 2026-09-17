#include "wifi_setup.h"

#include <WiFi.h>

#include "config.h"
#include "credentials.h"

bool wifiConnect() {
  Serial.printf("[wifi] Connecting to \"%s\"", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < WIFI_TIMEOUT_MS) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("[wifi] Connected. IP: ");
    Serial.println(WiFi.localIP());
    return true;
  }

  Serial.println("[wifi] Connection failed (timeout). Serial output still "
                 "works; MQTT and the web server will be unavailable.");
  return false;
}

bool wifiIsConnected() {
  return WiFi.status() == WL_CONNECTED;
}

String wifiIP() {
  return WiFi.localIP().toString();
}
