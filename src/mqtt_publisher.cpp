#include "mqtt_publisher.h"

#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "config.h"
#include "credentials.h"

static WiFiClientSecure wifiClient;
static PubSubClient mqttClient(wifiClient);
static unsigned long lastReconnectAttempt = 0;

void mqttInit() {
  // TLS transport for HiveMQ Cloud (port 8883). setInsecure() encrypts the
  // connection but does NOT verify the broker certificate. To enable full
  // verification later, replace this with wifiClient.setCACert(rootCA) and
  // add NTP time sync so the certificate validity dates can be checked.
  wifiClient.setInsecure();
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
}

bool mqttEnsureConnected() {
  if (mqttClient.connected()) {
    return true;
  }

  // Rate-limit reconnect attempts so the loop never blocks on the broker.
  unsigned long now = millis();
  if (now - lastReconnectAttempt < MQTT_RECONNECT_INTERVAL_MS) {
    return false;
  }
  lastReconnectAttempt = now;

  Serial.print("[mqtt] Connecting to broker... ");
  bool ok;
  if (strlen(MQTT_USER) > 0) {
    ok = mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD);
  } else {
    ok = mqttClient.connect(MQTT_CLIENT_ID);
  }

  if (ok) {
    Serial.println("connected.");
  } else {
    Serial.printf("failed, rc=%d (will retry).\n", mqttClient.state());
  }
  return ok;
}

void mqttLoop() {
  mqttClient.loop();
}

bool mqttPublish(const SensorReading& reading) {
  if (!reading.valid || !mqttClient.connected()) {
    return false;
  }

  JsonDocument doc;
  doc["temperature_c"] = reading.temperature_c;
  doc["pressure_hpa"] = reading.pressure_hpa;
  doc["altitude_m"] = reading.altitude_m;

  char payload[128];
  size_t len = serializeJson(doc, payload, sizeof(payload));

  bool ok = mqttClient.publish(MQTT_TOPIC, reinterpret_cast<const uint8_t*>(payload),
                               len, MQTT_RETAIN);
  if (ok) {
    Serial.printf("[mqtt] Published to %s: %s\n", MQTT_TOPIC, payload);
  } else {
    Serial.println("[mqtt] Publish failed.");
  }
  return ok;
}
