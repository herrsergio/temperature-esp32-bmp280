# CLAUDE.md

Guidance for Claude Code when working in this repository.

## What this project is

Firmware for an **ESP32-WROOM-32** (ESP32-D0WD-V3, dual core, 4MB flash) that
reads a **BMP280** sensor over **I2C** and reports temperature, pressure and
derived altitude via **serial**, **MQTT** (JSON) and an on-board **HTTP web
server**. Framework: **PlatformIO + Arduino (C++)**.

## Environment

- PlatformIO Core is installed in the project `.venv` (Python 3.14).
- Always invoke it from the venv: `.venv/bin/pio ...`.
- **No ESP32 is connected to this machine.** Do not attempt `upload` or
  `device monitor` here. The only local verification is compilation.

## Common commands

```bash
.venv/bin/pio run                 # Compile (primary check; no hardware needed)
.venv/bin/pio run -t clean        # Clean build artifacts
```

Upload/monitor commands exist in the README but require physical hardware; do
not run them in this environment.

## Architecture

Small single-purpose modules in `src/`, wired together in `main.cpp`:

- `sensor.{h,cpp}` — owns the `Adafruit_BMP280` instance; `sensorRead()`
  returns a `SensorReading { temperature_c, pressure_hpa, altitude_m, valid }`.
- `wifi_setup.{h,cpp}` — WiFi connection with timeout.
- `mqtt_publisher.{h,cpp}` — non-blocking MQTT connect + JSON publish.
- `web_server.{h,cpp}` — `WebServer` on port 80; caches the latest reading,
  serves `/` (HTML) and `/json`.

`main.cpp` runs network services every loop and takes a reading every
`READING_INTERVAL_MS`.

## Configuration

- **Non-secret** settings: `include/config.h` (pins, I2C address, intervals,
  MQTT topic, sea-level hPa, web port).
- **Secrets**: `include/credentials.h` (WiFi + MQTT broker). This file is
  **gitignored**. The committed template is `include/credentials.h.example`.
- A local `credentials.h` with placeholder values exists so the project
  compiles. Never commit `credentials.h`; never move secrets into `config.h`
  or source files.

## Conventions

- All code identifiers and comments in **English**.
- Do not use em-dashes in code or docs.
- Keep modules small and single-purpose. New responsibilities go in a new
  module pair rather than growing `main.cpp`.
- Default I2C address is `0x76`; `0x77` is the documented fallback.

## Verifying changes

After any code change, run `.venv/bin/pio run` and confirm a `[SUCCESS]`
build before claiming the change works. Hardware behavior cannot be verified
here, so state clearly that runtime testing on the board is still required.
