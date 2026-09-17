#include <Arduino.h>

#include "config.h"
#include "sensor.h"
#include "wifi_setup.h"
#include "mqtt_publisher.h"
#include "web_server.h"

static unsigned long lastReading = 0;

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(1000);
  Serial.println("\n[main] ESP32-WROOM-32 + BMP280 sensor reader starting.");

  if (!sensorInit()) {
    Serial.println("[main] Sensor init failed. Continuing; readings will be "
                   "reported as invalid.");
  }

  wifiConnect();  // Non-fatal: serial output works even without WiFi.
  mqttInit();
  webServerInit();
}

void loop() {
  // Keep network services responsive on every iteration.
  webServerHandle();
  mqttLoop();
  mqttEnsureConnected();

  unsigned long now = millis();
  if (now - lastReading >= READING_INTERVAL_MS || lastReading == 0) {
    lastReading = now;

    SensorReading reading = sensorRead();
    if (reading.valid) {
      Serial.printf("[reading] T=%.2f C  P=%.2f hPa  Alt=%.2f m\n",
                    reading.temperature_c, reading.pressure_hpa,
                    reading.altitude_m);
    } else {
      Serial.println("[reading] Invalid reading (sensor not available).");
    }

    webServerUpdateReading(reading);
    mqttPublish(reading);
  }
}
