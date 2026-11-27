/*
 * HC-SR04 Ultrasonic Distance & Parking Alert Sensor
 * TechBlueprint (@techblueprint-lab)
 *
 * Physics:
 *   Speed of sound in dry air (~20°C) = 343 m/s = 0.0343 cm/µs
 *   Because the 40 kHz sound pulse travels OUT to the object and BACK:
 *     distance_cm = (echo_duration_us * 0.0343) / 2.0
 *
 * Engineering Guardrails Included:
 *   1. 30 ms pulseIn() timeout — prevents the Arduino loop from freezing
 *      for a full 1 second if no echo returns (out of range / angled wall).
 *   2. 3-sample median filter — rejects random ultrasonic bounce glitches.
 *   3. 3-zone LED + progressive buzzer parking alert (Safe / Caution / Stop).
 */

const uint8_t TRIG_PIN   = 9;
const uint8_t ECHO_PIN   = 10;
const uint8_t LED_GREEN  = 3;
const uint8_t LED_YELLOW = 4;
const uint8_t LED_RED    = 5;
const uint8_t BUZZER_PIN = 6;

// 30,000 µs timeout ≈ 514 cm max round-trip (covers HC-SR04's 400 cm range)
const unsigned long ECHO_TIMEOUT_US = 30000UL;
const float SOUND_SPEED_CM_PER_US   = 0.0343f;

// Alert thresholds (in centimeters)
const float STOP_CM    = 15.0f;
const float CAUTION_CM = 35.0f;

float readSingleDistanceCm() {
  // Ensure a clean LOW state, then fire a 10 µs HIGH pulse on TRIG
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure how long ECHO stays HIGH (with 30 ms non-blocking safety timeout!)
  unsigned long durationUs = pulseIn(ECHO_PIN, HIGH, ECHO_TIMEOUT_US);
  if (durationUs == 0) {
    return -1.0f;  // Out of range or no echo received
  }

  // Divide by 2 because the 40 kHz sound wave makes a round trip
  return (durationUs * SOUND_SPEED_CM_PER_US) / 2.0f;
}

float readMedianDistanceCm() {
  // Take 3 quick readings and return the middle value to filter out echo noise
  float a = readSingleDistanceCm();
  delay(25);
  float b = readSingleDistanceCm();
  delay(25);
  float c = readSingleDistanceCm();

  // Sort 3 values cleanly without extra libraries
  if (a > b) { float t = a; a = b; b = t; }
  if (b > c) { float t = b; b = c; c = t; }
  if (a > b) { float t = a; a = b; b = t; }
  return b;
}

void setAlertState(bool greenOn, bool yellowOn, bool redOn) {
  digitalWrite(LED_GREEN,  greenOn  ? HIGH : LOW);
  digitalWrite(LED_YELLOW, yellowOn ? HIGH : LOW);
  digitalWrite(LED_RED,    redOn    ? HIGH : LOW);
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  setAlertState(false, false, false);
  Serial.println("HC-SR04 Ultrasonic Parking Sensor Ready (@techblueprint-lab)");
}

void loop() {
  float distCm = readMedianDistanceCm();

  if (distCm < 0.0f) {
    setAlertState(true, false, false);
    noTone(BUZZER_PIN);
    Serial.println("Distance: > 400 cm (Clear)");
    delay(120);
    return;
  }

  Serial.print("Distance: ");
  Serial.print(distCm, 1);
  Serial.println(" cm");

  if (distCm <= STOP_CM) {
    // Danger / Stop zone
    setAlertState(false, false, true);
    tone(BUZZER_PIN, 2400);
    delay(80);
  } else if (distCm <= CAUTION_CM) {
    // Caution zone: faster beeps as obstacle gets closer
    setAlertState(false, true, false);
    int beepDelay = map((int)distCm, (int)STOP_CM, (int)CAUTION_CM, 60, 260);
    tone(BUZZER_PIN, 1600, 60);
    delay(beepDelay);
  } else {
    // Safe zone
    setAlertState(true, false, false);
    noTone(BUZZER_PIN);
    delay(100);
  }
}
