# How an HC-SR04 Ultrasonic Sensor Works (+ Arduino Parking Alert)

Ever wondered how a **$1.50 ultrasonic sensor** measures distance with invisible sound — just like a bat?

This blueprint breaks down the physics of the **HC-SR04** module and gives you clean, glitch-resistant Arduino firmware for a 3-zone **Parking / Obstacle Alert Sensor**.

## How the Physics Works (Time of Flight)

1. Your Arduino sends a **10 µs HIGH pulse** to the `TRIG` pin.
2. The left metallic transducer (`T`) fires **8 ultrasonic sound pulses at 40 kHz** (double the 20 kHz limit of human hearing).
3. Those sound waves travel through air at **343 m/s (`0.0343 cm/µs`)**, hit an obstacle, and bounce back to the right receiver transducer (`R`).
4. The `ECHO` pin stays `HIGH` for the exact round-trip travel time in microseconds.
5. **Don't forget to divide by 2** (because the sound went *out* and came *back*):

```text
Distance (cm) = (Echo Time in µs × 0.0343) / 2
```

## Two Real Engineering Fixes Included in This Code

- **30 ms `pulseIn()` Timeout:** By default, Arduino's `pulseIn(ECHO, HIGH)` waits up to **1 full second** if no echo returns — freezing your entire robot or sensor loop. Passing `30000UL` (30 ms ≈ 5.1 m round-trip) caps the wait so your code stays fast and responsive.
- **3-Sample Median Filter:** Ultrasonic echoes occasionally bounce off angled walls and produce a random spike. Taking the middle of 3 quick readings eliminates false alarms without slowing down response time.

## Parts List (~$7 Total)

| Part | Est. Cost | Notes |
| :--- | :---: | :--- |
| Arduino Uno / Nano | $4.50 | 5V logic matches HC-SR04 directly |
| HC-SR04 Ultrasonic Sensor | $1.50 | Range: 2 cm – 400 cm, 40 kHz, 15° cone |
| 3× LEDs (Green, Yellow, Red) + 3× 220 Ω Resistors | $0.50 | Safe / Caution / Stop indicator |
| Active Piezo Buzzer + Breadboard & Jumpers | $0.50 | Progressive parking beep |

## Pin Wiring Table

| Component Pin | Arduino Pin |
| :--- | :---: |
| HC-SR04 `VCC` | `5V` |
| HC-SR04 `GND` | `GND` |
| HC-SR04 `TRIG` | `D9` |
| HC-SR04 `ECHO` | `D10` |
| Green LED (via 220 Ω) | `D3` |
| Yellow LED (via 220 Ω) | `D4` |
| Red LED (via 220 Ω) | `D5` |
| Buzzer (`+`) | `D6` |

## Quick Start

1. Wire the HC-SR04, LEDs, and buzzer using the table above.
2. Open [`ultrasonic_parking_sensor.ino`](ultrasonic_parking_sensor.ino) in Arduino IDE.
3. Upload to your Arduino Uno/Nano and open **Serial Monitor (`115200 baud`)**.

---

💻 Follow **`@techblueprint-lab`** for daily practical ML, Arduino & Raspberry Pi builds.
