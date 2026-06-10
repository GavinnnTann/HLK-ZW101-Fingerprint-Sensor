/*
 * HLK-ZW Fingerprint Sensor Library
 * Example: Continuous 1:N fingerprint matching
 *
 * Place a finger on the sensor — the matched ID and confidence score print
 * to the Serial monitor. The LED flashes green on a match and red on no-match.
 * RGB LED effects (0x3C) work on ZW1xx modules; the library automatically
 * falls back to simple on/off on passive-LED variants (ZW06xx/ZW09xx/ZW30xx).
 *
 * Wiring (ESP32):
 *   Sensor TX  →  GPIO 16  (ESP32 RX)
 *   Sensor RX  →  GPIO 17  (ESP32 TX)
 *   Sensor VCC →  3.3 V or 5 V (check your module's datasheet)
 *   Sensor GND →  GND
 *   CTRL       →  (optional) GPIO for low-power circuit enable; set CTRL below, -1 to disable
 */

#include <HLK_fingerprint.h>

#define FP_RX  16
#define FP_TX  17
constexpr int CTRL = -1;  // set to GPIO number to enable low-power circuit; -1 = disabled

FingerprintModule fp(Serial1, FP_RX, FP_TX);

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);
    Serial.println(F("\nHLK-ZW Fingerprint Match"));
    Serial.println(F("========================="));

    if (CTRL >= 0) {
        pinMode(CTRL, OUTPUT);
        digitalWrite(CTRL, HIGH);  // assert HIGH to enable low-power circuit
    }

    if (!fp.begin()) {
        Serial.println(F("ERROR: Module not found. Check wiring and baud rate."));
        while (true) delay(1000);
    }

    Serial.print(F("Module ready  |  "));
    Serial.print(fp.getTemplateCount());
    Serial.print(F(" / "));
    Serial.print(fp.capacity());
    Serial.println(F(" slots enrolled."));
    Serial.println(F("Place a finger on the sensor..."));

    fp.ledBreathing(FP_LED_BLUE);  // idle breathing animation
}

void loop() {
    uint16_t score = 0;
    int16_t  id    = fp.matchFingerprint(score);  // blocks up to 10 s waiting for finger

    if (id >= 0) {
        // ── Match ─────────────────────────────────────────────────────────────
        Serial.print(F("MATCH   ID: ")); Serial.print(id);
        Serial.print(F("   Confidence: ")); Serial.println(score);
        fp.ledFlash(FP_LED_GREEN, 2);
        delay(1000);
        fp.ledBreathing(FP_LED_BLUE);

    } else if (id == -1) {
        // ── No match ──────────────────────────────────────────────────────────
        Serial.println(F("NO MATCH"));
        fp.ledFlash(FP_LED_RED, 3);
        delay(1000);
        fp.ledBreathing(FP_LED_BLUE);

    }
    // id == -2: timed out waiting for a finger, or communication error — just retry
}
