/*
 * TechBlueprint (@techblueprint-lab) — Day 002: Smart Auto-Watering Greenhouse
 * Board   : Arduino Uno / Nano
 * Sensors : Capacitive Soil Moisture Sensor (A0) + DHT22 Temp/Humidity (D2)
 * Output  : 5V Relay + Mini Water Pump (D7) + Status LEDs (D8, D9, D10)
 *
 * Practical Engineering Highlights:
 *  1. Uses a CAPACITIVE soil sensor (resistive probes corrode in wet soil).
 *  2. Uses HYSTERESIS (pump ON below 35%, OFF above 55%) to prevent relay chatter.
 *  3. Includes a 10-second MAX-PUMP-RUNTIME safety cutoff + 30s cooldown so a
 *     disconnected sensor can never flood your room!
 */

#include <DHT.h>

// Pin Definitions
const int SOIL_PIN      = A0;  // Capacitive soil moisture analog output
const int DHT_PIN       = 2;   // DHT22 data pin (with 10k pull-up resistor)
const int RELAY_PIN     = 7;   // 5V Relay IN pin (active LOW on most modules)
const int LED_OK_PIN    = 8;   // Green LED: Soil healthy
const int LED_PUMP_PIN  = 9;   // Blue LED : Watering active
const int LED_HEAT_PIN  = 10;  // Red LED  : High temperature alert

// Calibration values (measure in dry air vs. glass of water for your sensor)
const int SOIL_DRY_RAW  = 590;
const int SOIL_WET_RAW  = 255;

// Control thresholds (Hysteresis avoids rapid ON/OFF clicking)
const int MOISTURE_LOW_PCT  = 35;  // Start watering below 35%
const int MOISTURE_HIGH_PCT = 55;  // Stop watering above 55%
const float TEMP_ALERT_C    = 32.0;

// Safety guardrails (Flood protection!)
const unsigned long MAX_PUMP_MS = 10000UL;  // Never pump > 10 sec continuously
const unsigned long COOLDOWN_MS = 30000UL;  // Wait 30 sec before next cycle

DHT dht(DHT_PIN, DHT22);

bool pumpRunning = false;
unsigned long pumpStartTime = 0;
unsigned long lastPumpStopTime = 0;

void setPump(bool on) {
  pumpRunning = on;
  // Most 5V relay modules are Active-LOW (LOW = ON, HIGH = OFF)
  digitalWrite(RELAY_PIN, on ? LOW : HIGH);
  digitalWrite(LED_PUMP_PIN, on ? HIGH : LOW);
  if (on) {
    pumpStartTime = millis();
  } else {
    lastPumpStopTime = millis();
  }
}

int readSoilMoisturePct() {
  // Average 5 readings for stable noise-free measurements
  long sum = 0;
  for (int i = 0; i < 5; i++) {
    sum += analogRead(SOIL_PIN);
    delay(15);
  }
  int raw = sum / 5;
  int pct = map(raw, SOIL_DRY_RAW, SOIL_WET_RAW, 0, 100);
  return constrain(pct, 0, 100);
}

void setup() {
  Serial.begin(9600);
  dht.begin();

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_OK_PIN, OUTPUT);
  pinMode(LED_PUMP_PIN, OUTPUT);
  pinMode(LED_HEAT_PIN, OUTPUT);

  // Start with pump safely OFF
  digitalWrite(RELAY_PIN, HIGH);
  Serial.println(F("TechBlueprint Smart Greenhouse Ready!"));
}

void loop() {
  unsigned long now = millis();
  int moisture = readSoilMoisturePct();
  float tempC = dht.readTemperature();
  float humidity = dht.readHumidity();

  // 1. Flood-safety watchdog: force pump OFF if max runtime exceeded
  if (pumpRunning && (now - pumpStartTime >= MAX_PUMP_MS)) {
    setPump(false);
    Serial.println(F("[SAFETY] Max pump runtime reached -> Pump OFF (cooldown)"));
  }

  // 2. Hysteresis watering control
  bool cooldownDone = (now - lastPumpStopTime >= COOLDOWN_MS) || (lastPumpStopTime == 0);
  if (!pumpRunning && moisture < MOISTURE_LOW_PCT && cooldownDone) {
    setPump(true);
    Serial.println(F("[ACTION] Soil dry -> Water pump ON"));
  } else if (pumpRunning && moisture >= MOISTURE_HIGH_PCT) {
    setPump(false);
    Serial.println(F("[ACTION] Target moisture reached -> Water pump OFF"));
  }

  // 3. Status LEDs
  digitalWrite(LED_OK_PIN, (!pumpRunning && moisture >= MOISTURE_LOW_PCT) ? HIGH : LOW);
  bool heatAlert = !isnan(tempC) && (tempC >= TEMP_ALERT_C);
  digitalWrite(LED_HEAT_PIN, heatAlert ? HIGH : LOW);

  // 4. Serial Telemetry Log
  Serial.print(F("Moisture: ")); Serial.print(moisture); Serial.print(F("% | "));
  if (!isnan(tempC)) {
    Serial.print(F("Temp: ")); Serial.print(tempC, 1); Serial.print(F("C | "));
    Serial.print(F("Humidity: ")); Serial.print(humidity, 0); Serial.print(F("% | "));
  }
  Serial.println(pumpRunning ? F("Pump: ON") : F("Pump: OFF"));

  delay(2000);
}
