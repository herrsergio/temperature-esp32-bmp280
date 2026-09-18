#include "web_server.h"

#include <WebServer.h>
#include <ArduinoJson.h>

#include "config.h"

static WebServer server(WEB_SERVER_PORT);

// Most recent reading, updated by main.cpp and served to HTTP clients.
static SensorReading latest{};

// Single-page UI that fetches /json every 5 seconds and updates the values.
static const char INDEX_HTML[] PROGMEM = R"HTML(<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32 BMP280</title>
  <style>
    body { font-family: system-ui, sans-serif; margin: 2rem; background: #0f172a; color: #e2e8f0; }
    h1 { font-size: 1.25rem; }
    .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(160px, 1fr)); gap: 1rem; max-width: 640px; }
    .card { background: #1e293b; border-radius: 12px; padding: 1rem 1.25rem; }
    .label { font-size: .8rem; color: #94a3b8; text-transform: uppercase; letter-spacing: .05em; }
    .value { font-size: 1.75rem; font-weight: 600; margin-top: .25rem; }
    .stale { color: #f59e0b; }
  </style>
</head>
<body>
  <h1>ESP32-WROOM-32 + BMP280</h1>
  <p id="status" class="label">Loading...</p>
  <div class="grid">
    <div class="card"><div class="label">Temperature</div><div class="value"><span id="t">--</span> &deg;C</div></div>
    <div class="card"><div class="label">Pressure</div><div class="value"><span id="p">--</span> hPa</div></div>
    <div class="card"><div class="label">Altitude</div><div class="value"><span id="a">--</span> m</div></div>
  </div>
  <script>
    async function refresh() {
      try {
        const r = await fetch('/json');
        const d = await r.json();
        const status = document.getElementById('status');
        if (d.valid) {
          document.getElementById('t').textContent = d.temperature_c.toFixed(2);
          document.getElementById('p').textContent = d.pressure_hpa.toFixed(2);
          document.getElementById('a').textContent = d.altitude_m.toFixed(2);
          status.textContent = 'Updated ' + new Date().toLocaleTimeString();
          status.className = 'label';
        } else {
          status.textContent = 'Sensor unavailable';
          status.className = 'label stale';
        }
      } catch (e) {
        document.getElementById('status').textContent = 'Fetch error';
      }
    }
    refresh();
    setInterval(refresh, 5000);
  </script>
</body>
</html>)HTML";

static void handleRoot() {
  server.send_P(200, "text/html", INDEX_HTML);
}

static void handleJson() {
  JsonDocument doc;
  doc["valid"] = latest.valid;
  doc["temperature_c"] = latest.temperature_c;
  doc["pressure_hpa"] = latest.pressure_hpa;
  doc["altitude_m"] = latest.altitude_m;

  String out;
  serializeJson(doc, out);
  server.send(200, "application/json", out);
}

static void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

void webServerInit() {
  server.on("/", handleRoot);
  server.on("/json", handleJson);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.printf("[web] HTTP server started on port %d.\n", WEB_SERVER_PORT);
}

void webServerHandle() {
  server.handleClient();
}

void webServerUpdateReading(const SensorReading& reading) {
  latest = reading;
}
