/*
 * ESP32 + BMP180 Sensor Telemetry Simulation
 * 
 * What this does:
 *   - Reads temperature, pressure, and altitude from a BMP180 sensor
 *   - Prints the data to the serial monitor every 2 seconds
 * 
 * Why this matters for aerospace:
 *   - Pressure -> altitude is how aircraft and rockets measure height
 *   - This same sensor pattern (MCU + sensor + serial output) is used
 *     in CubeSat housekeeping systems and rocket telemetry
 * 
 * Hardware:
 *   - ESP32 DevKit (microcontroller)
 *   - BMP180 (temperature + pressure + altitude sensor)
 *   - I2C connection: SDA=GPIO21, SCL=GPIO22
 */

#include <Wire.h>              // I2C communication library
#include <Adafruit_BMP085.h>   // BMP180/085 sensor library

// Create a BMP180 object. We'll use it to talk to the sensor.
Adafruit_BMP085 bmp;

// --- Flight tracking variables ---
float maxAltitude = 0.0;              // Highest altitude seen so far
int descentCount = 0;                 // Consecutive readings below maxAltitude
bool apogeeDetected = false;          // Have we already announced apogee?

const float APOGEE_TOLERANCE = 0.5;      // Meters — ignore jitter smaller than this
const int   APOGEE_COUNT_THRESHOLD = 3;  // Need 3 consecutive "descending" reads

void setup() {
  // Start the serial monitor at 115200 baud.
  Serial.begin(115200);
  delay(1000);

  Serial.println("ESP32 + BMP180 Telemetry Simulation");
  Serial.println("-----------------------------------");

  // Initialize I2C. On ESP32, default pins are SDA=21, SCL=22.
  Wire.begin(21, 22);

  // Try to initialize the BMP180 sensor.
  if (!bmp.begin()) {
    Serial.println("ERROR: BMP180 not found. Check wiring.");
    while (1) {
      delay(10);
    }
  }

  Serial.println("BMP180 initialized successfully.");
  Serial.println();
}

// --- Simulated flight profile (for testing) ---
// This fake altitude climbs, peaks, then descends — so we can verify
// the apogee detection logic works without changing physical sensors.
float simulatedAltitude = 0.0;
int   flightStep = 0;

void loop() {
  // --- Fly the fake altitude profile ---
  // Each step corresponds to one loop iteration (every 500ms).
  // Profile: climb to 300m, then descend.
  switch (flightStep) {
    case 0: simulatedAltitude = 0.0;    break;  // pad
    case 1: simulatedAltitude = 50.0;   break;  // ascent
    case 2: simulatedAltitude = 120.0;  break;
    case 3: simulatedAltitude = 200.0;  break;
    case 4: simulatedAltitude = 280.0;  break;
    case 5: simulatedAltitude = 300.0;  break;  // apogee region
    case 6: simulatedAltitude = 295.0;  break;  // descent begins
    case 7: simulatedAltitude = 280.0;  break;
    case 8: simulatedAltitude = 250.0;  break;
    default: simulatedAltitude = 200.0; break;  // falling further
  }
  flightStep++;

  // --- Still read the real sensor so we can see it in the log ---
  float temperature = bmp.readTemperature();
  float pressure    = bmp.readPressure();
  // float altitude = bmp.readAltitude();  // real sensor — replaced by simulation

  // --- Use the simulated altitude instead of the real one ---
  float altitude = simulatedAltitude;

  float time_s = millis() / 1000.0;

  // --- Update max altitude ---
  if (altitude > maxAltitude) {
    maxAltitude = altitude;
  }

  // --- Apogee detection ---
  if (altitude < (maxAltitude - APOGEE_TOLERANCE)) {
    descentCount++;
  } else {
    descentCount = 0;
  }

  if (descentCount >= APOGEE_COUNT_THRESHOLD && !apogeeDetected) {
    Serial.print(">>> APOGEE DETECTED at t=");
    Serial.print(time_s, 1);
    Serial.print("s | MAX ALT: ");
    Serial.print(maxAltitude, 1);
    Serial.println("m");
    apogeeDetected = true;
  }

  // --- Print telemetry frame ---
  Serial.print("[t=");
  Serial.print(time_s, 1);
  Serial.print("s] ALT: ");
  Serial.print(altitude, 1);
  Serial.print("m | MAX: ");
  Serial.print(maxAltitude, 1);
  Serial.print("m | TEMP: ");
  Serial.print(temperature, 1);
  Serial.print("C | PRESSURE: ");
  Serial.print(pressure / 100.0, 1);
  Serial.println("hPa");

  delay(500);
}
