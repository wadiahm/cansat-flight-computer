# CanSat Flight Computer — ESP32 + BMP180

A simulated flight computer that reads barometric pressure, computes altitude, and autonomously detects apogee — the peak of a rocket's flight.

Built in [Wokwi](https://wokwi.com) — no physical hardware required.

---

## What It Does

- Reads temperature, pressure, and altitude from a **BMP180** sensor over **I2C**
- Tracks maximum altitude reached during flight
- Detects **apogee** (peak altitude) using noise-filtered descent logic
- Prints structured, timestamped telemetry frames to the serial monitor

---

## Why Apogee Detection Is Non-Trivial

Sensor readings jitter. The BMP180 fluctuates ±0.2m even when stationary. A naive check like:

if (altitude < lastAltitude) → apogee!


...triggers false positives on every tiny fluctuation. On a real rocket, that would deploy the parachute mid-ascent — tearing it off and destroying the vehicle.

### The Solution

Three-stage filtering:

1. **Track max altitude** — remember the highest reading seen so far
2. **Tolerance band (0.5m)** — ignore drops smaller than this; they're just noise
3. **Consecutive counter (3 readings)** — require three readings in a row that are below `maxAltitude - tolerance` before declaring apogee

Once triggered, `apogeeDetected = true` locks the event — no spam, no re-triggering.

This pattern (tolerance + consecutive confirmation) is standard in embedded systems for making reliable decisions from noisy sensor data.

---

## Hardware

| Component | Purpose |
|-----------|---------|
| ESP32 DevKit | Microcontroller — reads sensor, runs logic, outputs telemetry |
| BMP180 | Barometric pressure + temperature sensor |
| I2C connection | SDA = GPIO21, SCL = GPIO22 |

**Wiring:**

| BMP180 | ESP32 |
|--------|-------|
| VCC | 3V3 |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

---

## Sample Output

[t=3.2s] ALT: 200.0m | MAX: 200.0m | TEMP: 24.0C | PRESSURE: 1000.0hPa
[t=3.8s] ALT: 280.0m | MAX: 280.0m | TEMP: 24.0C | PRESSURE: 1000.0hPa
[t=4.3s] ALT: 300.0m | MAX: 300.0m | TEMP: 24.0C | PRESSURE: 1000.0hPa
[t=4.8s] ALT: 295.0m | MAX: 300.0m | TEMP: 24.0C | PRESSURE: 1000.0hPa
[t=5.4s] ALT: 280.0m | MAX: 300.0m | TEMP: 24.0C | PRESSURE: 1000.0hPa
APOGEE DETECTED at t=5.9s | MAX ALT: 300.0m
[t=5.9s] ALT: 250.0m | MAX: 300.0m | TEMP: 24.0C | PRESSURE: 1000.0hPa


The altitude climbs to 300m, holds briefly, then descends. After 3 consecutive descending readings, apogee fires — once.

---

## How to Run It

1. Open [Wokwi](https://wokwi.com) and create a new ESP32 project
2. Add a **BMP180** component from the part picker
3. Wire it per the table above
4. Paste the contents of `sketch.ino`
5. Install the `Adafruit BMP085` library via the Library Manager
6. Click **▶ Play**
7. Watch the serial monitor

---

## Technologies

- **Arduino C++** (embedded)
- **I2C protocol** (2-wire master/slave)
- **Adafruit BMP085** library
- **Wokwi** simulator

---

## Why This Matters for Aerospace

- **Pressure → altitude** is how aircraft, rockets, and satellites measure height
- **Apogee detection** is the primary autonomous decision a rocket flight computer makes — it triggers parachute deployment
- The **sensor → MCU → telemetry** pattern used here is the same architecture found in CubeSat housekeeping systems and rocket flight computers

---

## What I Learned

- I2C communication protocol (2-wire master/slave, device addresses)
- Sensor initialization, polling, and data conversion
- Noise filtering in real-world sensor data
- Autonomous event detection (tolerance + consecutive counters)
- Structured telemetry formatting with timestamps
- Embedded debugging via serial output

---

## Limitations & Honest Notes

- This is a **simulation**, not real hardware. The Wokwi ESP32 and BMP180 are software models.
- Real spacecraft use **radiation-hardened ICs**, not consumer ESP32s.
- Real telemetry protocols (e.g., CCSDS) are far more complex than the serial output here.
- The "flight" is a **pre-programmed altitude profile**, not physical motion. This is standard practice for testing flight software before hardware is available.

---

## Related Concepts

- **CanSat** — a miniaturized satellite built inside a soda-can-sized container, used in student aerospace competitions
- **Apogee** — the highest point of a rocket's trajectory before descent begins
- **Housekeeping data** — onboard sensor readings a satellite transmits to ground stations to report its health

