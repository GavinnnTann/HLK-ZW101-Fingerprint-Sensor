/*
 * HLK-ZW Fingerprint Sensor Library
 * Example: ESP32 USB-CDC ↔ UART Bridge  (MCU Adapter)
 *
 * Use this sketch when you do NOT have a dedicated USB-to-serial adapter
 * (CH340, CP2102, FT232, etc.). Flash it onto an ESP32 and it transparently
 * bridges bytes between the PC and the fingerprint sensor, so the
 * HLK-ZW101 Tester Python application can communicate with the module
 * exactly as if a hardware USB-serial adapter were connected.
 *
 * How it works:
 *   PC  ←USB CDC→  ESP32 (Serial)  ←UART→  FP sensor (Serial1)
 *
 * Setup:
 *   1. Wire the fingerprint sensor to the ESP32 (see pin definitions below).
 *   2. Flash this sketch onto your ESP32.
 *   3. Open the HLK-ZW101 Tester, select the ESP32's COM port, set baud to
 *      57600, and click Connect — the tester will talk to the sensor through
 *      the ESP32 transparently.
 *
 * Wiring:
 *   Sensor TX  →  GPIO FP_RX  (ESP32 receives)
 *   Sensor RX  →  GPIO FP_TX  (ESP32 transmits)
 *   Sensor VCC →  3.3 V or 5 V (check your module's datasheet)
 *   Sensor GND →  GND
 *
 * CTRL pin (optional):
 *   Driven HIGH on startup. Connect to a sensor power-enable line, an LED
 *   status indicator, or leave unconnected if unused.
 *
 * Notes:
 *   - Both Serial (USB) and Serial1 (sensor) must run at the same baud rate.
 *     The sensor default is 57600. If you have changed the sensor baud rate
 *     via the Tester settings, update both Serial.begin() and Serial1.begin()
 *     below to match.
 *   - The ESP32 USB-CDC port may reset the board when the PC opens it.
 *     If the tester times out on first connect, disconnect and reconnect once.
 */

#include <Arduino.h>

#define FP_RX 7  // adjust to your wiring
#define FP_TX 6
constexpr int CTRL = -1;  // set to a GPIO number to enable low-power circuit control; -1 = disabled

void setup() {
  Serial.begin(57600);                              // USB CDC to PC
  Serial1.begin(57600, SERIAL_8N1, FP_RX, FP_TX);  // FP sensor default baud
  if (CTRL >= 0) {
    pinMode(CTRL, OUTPUT);
    digitalWrite(CTRL, HIGH);  // assert HIGH to enable low-power circuit
  }
}

void loop() {
  if (Serial.available())  Serial1.write(Serial.read());
  if (Serial1.available()) Serial.write(Serial1.read());
}