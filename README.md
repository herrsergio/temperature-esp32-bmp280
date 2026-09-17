# temperature-esp32-bmp280

Read temperature, pressure and derived altitude from a **BMP280** sensor on an
**ESP32-WROOM-32** (ESP32-D0WD-V3, dual core, 4MB flash). Readings are:

- printed to the **serial** monitor,
- published as **JSON to an MQTT** broker, and
- served from an on-board **HTTP web server** (a live page plus a `/json` endpoint).

Built with **PlatformIO + Arduino** (C++).

## Hardware

### Wiring (I2C)

The BMP280 is connected over I2C. Default ESP32 I2C pins are used.

| BMP280 pin | ESP32-WROOM-32 pin | Notes                          |
| ---------- | ------------------ | ------------------------------ |
| VCC        | 3V3                | 3.3 V, do not use 5 V          |
| GND        | GND                |                                |
| SCL        | GPIO22             | I2C clock                      |
| SDA        | GPIO21             | I2C data                       |
| CSB        | (leave unconnected)| Pulled high = I2C mode         |
| SDO        | GND or 3V3         | Sets address: GND = 0x76, 3V3 = 0x77 |

Most GY-BMP280 breakout boards default to address **0x76**. If the sensor is
not detected, change `BMP280_I2C_ADDRESS` to `0x77` in `include/config.h`.

## Software setup

PlatformIO Core is installed into the project `.venv`.

```bash
# Install PlatformIO (already done if .venv has it)
.venv/bin/pip install platformio

# Verify
.venv/bin/pio --version
```

> Note: this project was built and compiled successfully with PlatformIO Core
> 6.2.0 on Python 3.14. If a future PlatformIO or toolchain version rejects
> Python 3.14, create a venv with Python 3.11-3.13 and install PlatformIO there,
> or use `pipx install platformio`.

### Configure credentials

Secrets are kept out of git. Copy the template and fill in your values:

```bash
cp include/credentials.h.example include/credentials.h
# then edit include/credentials.h
```

`include/credentials.h` is gitignored. Non-secret settings (pins, intervals,
I2C address, MQTT topic, sea-level reference pressure) live in
`include/config.h`.

## Build, flash, monitor

```bash
# Compile only (no board required)
.venv/bin/pio run

# Flash to a connected ESP32
.venv/bin/pio run -t upload

# Open the serial monitor at 115200 baud
.venv/bin/pio device monitor -b 115200
```

## Usage

Once flashed and powered, the ESP32:

1. Connects to WiFi (2.4 GHz only; the WROOM-32 has no 5 GHz radio).
2. Reads the BMP280 every 10 s (configurable via `READING_INTERVAL_MS`).
3. Prints each reading to serial:
   ```
   [reading] T=24.31 C  P=1012.87 hPa  Alt=3.62 m
   ```
4. Publishes JSON to the MQTT topic `esp32/bmp280`:
   ```json
   {"temperature_c":24.31,"pressure_hpa":1012.87,"altitude_m":3.62}
   ```
5. Serves a live web page. Find the device IP in the serial log, then open
   `http://<esp32-ip>/` in a browser, or query `http://<esp32-ip>/json`.

### Check MQTT from a PC

```bash
mosquitto_sub -h <broker-host> -t 'esp32/bmp280'
```

## Project layout

```
platformio.ini            PlatformIO env (esp32dev, Arduino, libraries)
include/
  config.h                Non-secret configuration
  credentials.h.example   Secrets template (committed)
  credentials.h           Real secrets (gitignored)
src/
  main.cpp                Orchestration: setup + timed loop
  sensor.{h,cpp}          BMP280 read into a SensorReading struct
  wifi_setup.{h,cpp}      WiFi connection
  mqtt_publisher.{h,cpp}  MQTT connect + JSON publish
  web_server.{h,cpp}      HTTP server: "/" page and "/json"
```

## Notes

- Altitude is derived from pressure using `SEALEVEL_HPA` (default 1013.25).
  Set it to your local QNH in `include/config.h` for an accurate altitude.
- WiFi/MQTT failures are non-fatal: serial readings keep working, and the MQTT
  client reconnects on its own without blocking the loop.

## Out of scope

Deep sleep / low power, OTA updates, TLS (MQTTS), multiple sensors, and
historical storage are intentionally not included.
