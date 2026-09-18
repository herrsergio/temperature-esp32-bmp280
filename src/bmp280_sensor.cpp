#include "bmp280_sensor.h"

#include <Wire.h>
#include <Adafruit_BMP280.h>

#include "config.h"

// BMP280 driver instance using the default I2C bus (Wire).
static Adafruit_BMP280 bmp;
static bool sensorReady = false;

bool sensorInit() {
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

  if (!bmp.begin(BMP280_I2C_ADDRESS)) {
    Serial.println("[sensor] BMP280 not found. Check wiring and I2C address "
                   "(try 0x77 if 0x76 fails).");
    sensorReady = false;
    return false;
  }

  // Recommended settings for weather monitoring (slow, oversampled, filtered).
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_X2,    // temperature oversampling
                  Adafruit_BMP280::SAMPLING_X16,   // pressure oversampling
                  Adafruit_BMP280::FILTER_X16,     // IIR filter
                  Adafruit_BMP280::STANDBY_MS_500);

  sensorReady = true;
  Serial.println("[sensor] BMP280 initialized.");
  return true;
}

SensorReading sensorRead() {
  SensorReading r{};

  if (!sensorReady) {
    r.valid = false;
    return r;
  }

  r.temperature_c = bmp.readTemperature();
  r.pressure_hpa = bmp.readPressure() / 100.0f;  // Pa -> hPa
  r.altitude_m = bmp.readAltitude(SEALEVEL_HPA);
  r.valid = !isnan(r.temperature_c) && !isnan(r.pressure_hpa);

  return r;
}
