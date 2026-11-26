# Day 002 — Smart Auto-Watering Greenhouse (Arduino)

A practical **~$14 Arduino prototype** that automatically monitors soil moisture and temperature, waters your plants only when needed, and protects your desk/floor from accidental flooding.

## 3 Real Engineering Lessons Inside This Build
1. **Use a Capacitive Soil Sensor (Not Resistive):** Cheap bare-metal resistive probes electrolyze and corrode within weeks in wet soil. Capacitive sensors (`v1.2` / `v2.0`) measure dielectric change without exposed copper pads, lasting way longer.
2. **Use Hysteresis (`< 35%` ON → `> 55%` OFF):** If you use a single threshold like `< 40%`, sensor noise makes your relay rapidly click ON/OFF/ON/OFF and burn out. Hysteresis creates a clean deadband.
3. **Add a Flood-Safety Cutoff Timer (`10s` max + `30s` cooldown):** If the soil probe ever slips out of the pot, naive code runs the pump forever until your floor floods. Our `millis()` watchdog caps every watering burst to 10 seconds!

---

## Parts List (BOM — ~$14 Total)

| Component | Qty | Est. Cost (USD) | Notes |
| :--- | :---: | :---: | :--- |
| **Arduino Uno** (or Nano) | 1 | $4.50 | Runs the control loop & safety timer |
| **Capacitive Soil Moisture Sensor v1.2** | 1 | $1.50 | Analog output → `A0` (corrosion-resistant) |
| **DHT22 Temp & Humidity Sensor** | 1 | $2.50 | Digital data → `D2` (with 10kΩ pull-up) |
| **5V 1-Channel Relay Module** | 1 | $1.50 | Control pin → `D7` (Active-LOW) |
| **Mini 5V Submersible Water Pump + Tubing** | 1 | $3.00 | Powered via relay + separate 5V supply |
| **LEDs (Green, Blue, Red) + 220Ω Resistors** | 3 | $1.00 | Status indicators (`D8`, `D9`, `D10`) |
| **Total** | — | **~$14.00** | |

---

## Pin Wiring Table

| Arduino Pin | Connected To | Purpose |
| :---: | :--- | :--- |
| **`A0`** | Capacitive Soil Sensor (`AOUT`) | Reads analog soil moisture |
| **`D2`** | DHT22 (`DATA`) | Reads temperature (°C) & humidity (%) |
| **`D7`** | 5V Relay Module (`IN`) | Switches water pump ON (`LOW`) / OFF (`HIGH`) |
| **`D8`** | Green LED (+ 220Ω resistor) | Healthy moisture indicator |
| **`D9`** | Blue LED (+ 220Ω resistor) | Pump active indicator |
| **`D10`** | Red LED (+ 220Ω resistor) | High temperature alert (`>= 32°C`) |
| **`5V` / `GND`** | Sensor VCC / GND rails | Power for sensors (power pump from external 5V!) |

> **Hardware Tip:** Always power the DC water pump through the relay's **COM / NO** terminals using a **separate 5V USB/battery supply** rather than drawing motor current directly from the Arduino `5V` pin — this prevents brownout resets!

---

## Quick Setup (Arduino IDE)
1. Install the **`DHT sensor library`** by Adafruit via **Sketch → Include Library → Manage Libraries**.
2. Open [`smart_greenhouse.ino`](smart_greenhouse.ino).
3. Calibrate `SOIL_DRY_RAW` (sensor in dry air) and `SOIL_WET_RAW` (sensor in a glass of water) via Serial Monitor (`9600 baud`).
4. Upload to your Arduino Uno/Nano and watch live telemetry!
