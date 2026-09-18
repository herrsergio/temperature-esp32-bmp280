# temperature-esp32-bmp280

Read temperature, pressure and derived altitude from a **BMP280** sensor on an
**ESP32-WROOM-32** (ESP32-D0WD-V3, dual core, 4MB flash). Readings are:

- printed to the **serial** monitor,
- published as **JSON over TLS to an MQTT broker** (e.g. HiveMQ Cloud), and
- served from an on-board **HTTP web server** (a live page plus a `/json` endpoint).

Built with **PlatformIO + Arduino** (C++).

## Hardware

### Wiring (I2C)

The BMP280 is connected over I2C. Default ESP32 I2C pins are used. This
project targets the common 4-pin (I2C-only) breakout with pins VDC, GND, SCL
and SDA.

| BMP280 pin | ESP32-WROOM-32 pin | Notes            |
| ---------- | ------------------ | ---------------- |
| VDC (VCC)  | 3V3                | 3.3 V power      |
| GND        | GND                |                  |
| SCL        | GPIO22             | I2C clock        |
| SDA        | GPIO21             | I2C data         |

On a 4-pin board the I2C address is fixed on the board (there is no SDO pin to
change it). It is usually **0x76**, which is the default in `include/config.h`.
If the sensor is not detected, set `BMP280_I2C_ADDRESS` to `0x77`. To find the
actual address, run an I2C scanner sketch and use whatever address responds.

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

#### HiveMQ Cloud

The firmware connects over **TLS on port 8883**, which HiveMQ Cloud's free
Serverless tier requires. In the HiveMQ Cloud console:

1. Open your cluster and copy the host from **Overview > Connection Settings**
   (it looks like `xxxxxxxx.s1.eu.hivemq.cloud`).
2. Create a device username/password under **Access Management**.
3. Fill those into `include/credentials.h`:
   ```cpp
   #define MQTT_HOST     "xxxxxxxx.s1.eu.hivemq.cloud"
   #define MQTT_PORT     8883
   #define MQTT_USER     "your-hivemq-username"
   #define MQTT_PASSWORD "your-hivemq-password"
   ```

Security note: the firmware calls `WiFiClientSecure::setInsecure()`, so traffic
is encrypted but the broker certificate is not verified. For full verification,
set the root CA via `setCACert()` and add NTP time sync in
`src/mqtt_publisher.cpp`.

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

The easiest verification is the **Web Client** in the HiveMQ Cloud console, or a
GUI like **MQTT Explorer** (both handle TLS with a checkbox). Subscribe to
`esp32/bmp280`; because messages are published retained, you get the last value
immediately.

With the `mosquitto_sub` CLI, TLS on 8883 needs a CA file (path is OS-dependent):

```bash
mosquitto_sub -h xxxxxxxx.s1.eu.hivemq.cloud -p 8883 \
  -u 'your-hivemq-username' -P 'your-hivemq-password' \
  -t 'esp32/bmp280' --cafile /etc/ssl/certs/ca-certificates.crt -v
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
  bmp280_sensor.{h,cpp}   BMP280 read into a SensorReading struct
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

Deep sleep / low power, OTA updates, full MQTT broker certificate verification
(the firmware encrypts but does not validate the server cert), multiple sensors,
and historical storage are intentionally not included.
