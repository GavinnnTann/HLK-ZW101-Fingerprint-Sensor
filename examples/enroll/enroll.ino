/*
 * HLK-ZW Fingerprint Sensor Library
 * Example: Enroll a new fingerprint
 *
 * Open the Serial monitor at 115200 baud. Enter the ID slot to enroll into,
 * then follow the prompts to scan the same finger twice.
 *
 * Wiring (ESP32):
 *   Sensor TX  →  GPIO 16  (ESP32 RX)
 *   Sensor RX  →  GPIO 17  (ESP32 TX)
 *   Sensor VCC →  3.3 V or 5 V (check your module's datasheet)
 *   Sensor GND →  GND
 *   CTRL       →  (optional) GPIO for low-power circuit enable; set CTRL below, -1 to disable
 *
 * Compatible modules: HLK-ZW101, ZW111, ZW06xx, ZW09xx, ZW30xx
 * All use the EF-01 UART protocol at 57600 baud (default).
 *
 * Tip: fp.enrollFingerprint(id) is a one-liner that does all steps below
 *      automatically. This example shows the steps explicitly for learning.
 */

#include <HLK_fingerprint.h>

#define FP_RX  16
#define FP_TX  17
constexpr int CTRL = -1;  // set to GPIO number to enable low-power circuit; -1 = disabled

FingerprintModule fp(Serial1, FP_RX, FP_TX);

// ── Helpers ──────────────────────────────────────────────────────────────────

// Wait for a line of serial input and return the parsed integer.
uint16_t readSerialInt() {
    while (!Serial.available()) delay(1);
    uint16_t v = (uint16_t)Serial.parseInt();
    while (Serial.available()) Serial.read();  // flush rest of line
    return v;
}

// ── Step-by-step enrollment ───────────────────────────────────────────────────

bool doEnroll(uint16_t id) {
    // ── Scan 1 ───────────────────────────────────────────────────────────────
    Serial.println(F("Place finger on sensor..."));
    while (!fp.getImage()) {
        if (fp.lastCC != 0x02) {
            Serial.print(F("Image error: 0x")); Serial.println(fp.lastCC, HEX);
            return false;
        }
    }
    Serial.println(F("  [✓] Scan 1 captured"));

    if (!fp.image2Tz(1)) {
        Serial.print(F("  Feature extraction failed: 0x")); Serial.println(fp.lastCC, HEX);
        return false;
    }

    // ── Wait for lift ─────────────────────────────────────────────────────────
    Serial.println(F("Remove finger..."));
    delay(500);
    do { delay(100); fp.getImage(); } while (fp.lastCC != 0x02);

    // ── Scan 2 ───────────────────────────────────────────────────────────────
    Serial.println(F("Place the SAME finger again..."));
    while (!fp.getImage()) {
        if (fp.lastCC != 0x02) {
            Serial.print(F("Image error: 0x")); Serial.println(fp.lastCC, HEX);
            return false;
        }
    }
    Serial.println(F("  [✓] Scan 2 captured"));

    if (!fp.image2Tz(2)) {
        Serial.print(F("  Feature extraction failed: 0x")); Serial.println(fp.lastCC, HEX);
        return false;
    }

    // ── Merge and store ───────────────────────────────────────────────────────
    Serial.println(F("Merging scans..."));
    if (!fp.createModel()) {
        if (fp.lastCC == 0x0A)
            Serial.println(F("  ERROR: Scans did not match — use the same finger both times."));
        else {
            Serial.print(F("  Model error: 0x")); Serial.println(fp.lastCC, HEX);
        }
        return false;
    }

    Serial.print(F("Storing as ID ")); Serial.print(id); Serial.println(F("..."));
    if (!fp.storeTemplate(1, id)) {
        Serial.print(F("  Store failed: 0x")); Serial.println(fp.lastCC, HEX);
        return false;
    }

    return true;
}

// ── Setup / Loop ──────────────────────────────────────────────────────────────

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);
    Serial.println(F("\nHLK-ZW Fingerprint Enrollment"));
    Serial.println(F("=============================="));

    if (CTRL >= 0) {
        pinMode(CTRL, OUTPUT);
        digitalWrite(CTRL, HIGH);  // assert HIGH to enable low-power circuit
    }

    if (!fp.begin()) {
        Serial.println(F("ERROR: Module not found. Check wiring and baud rate."));
        while (true) delay(1000);
    }

    Serial.print(F("Module ready  |  Capacity: "));
    Serial.print(fp.capacity());
    Serial.print(F(" slots  |  Enrolled: "));
    Serial.println(fp.getTemplateCount());
}

void loop() {
    Serial.println();
    Serial.print(F("Enter ID to enroll (0 - "));
    Serial.print(fp.capacity() - 1);
    Serial.print(F("): "));

    uint16_t id = readSerialInt();
    Serial.println(id);

    if (id >= fp.capacity()) {
        Serial.println(F("ID out of range — try again."));
        return;
    }

    if (fp.templateExists(id)) {
        Serial.print(F("WARNING: slot ")); Serial.print(id);
        Serial.println(F(" already has a template — it will be overwritten."));
    }

    Serial.println();
    if (doEnroll(id)) {
        Serial.print(F("\n[OK] Enrolled as ID ")); Serial.println(id);
        fp.ledFlash(FP_LED_GREEN, 2);  // visual confirmation (auto-falls back on passive LED variants)
    } else {
        Serial.println(F("\n[FAIL] Enrollment failed — please try again."));
        fp.ledFlash(FP_LED_RED, 2);
    }
}
