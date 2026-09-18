#pragma once
#include <Arduino.h>

// A single BMP280 measurement.
struct SensorReading {
  float temperature_c;  // Celsius
  float pressure_hpa;   // hectopascals
  float altitude_m;     // meters, derived from pressure and SEALEVEL_HPA
  bool valid;           // false when the sensor is missing or a read failed
};

// Initialize the I2C bus and the BMP280. Returns false if the sensor is
// not found at the configured address.
bool sensorInit();

// Take a reading. When the sensor is unavailable, the returned struct has
// valid == false and the numeric fields should be ignored.
SensorReading sensorRead();
