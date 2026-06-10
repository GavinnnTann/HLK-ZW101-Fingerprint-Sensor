/*
 * HLK-ZW Fingerprint Sensor Library
 * Example: Delete a fingerprint template
 *
 * Open the Serial monitor at 115200 baud.
 *   - Enter an ID (0 – capacity-1) to delete that single slot.
 *   - Enter a range as "first last" (e.g. "5 10") to delete slots 5 through 10.
 *   - Enter 9999 to wipe the entire library.
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

// ── Helpers ──────────────────────────────────────────────────────────────────

void printCount() {
    Serial.print(F("Enrolled: "));
    Serial.print(fp.getTemplateCount());
    Serial.print(F(" / "));
    Serial.println(fp.capacity());
}

// ── Setup / Loop ──────────────────────────────────────────────────────────────

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);
    Serial.println(F("\nHLK-ZW Delete Fingerprint"));
    Serial.println(F("=========================="));

    if (CTRL >= 0) {
        pinMode(CTRL, OUTPUT);
        digitalWrite(CTRL, HIGH);  // assert HIGH to enable low-power circuit
    }

    if (!fp.begin()) {
        Serial.println(F("ERROR: Module not found. Check wiring and baud rate."));
        while (true) delay(1000);
    }

    Serial.print(F("Module ready  |  "));
    printCount();
}

void loop() {
    Serial.println();
    Serial.print(F("Enter ID to delete (0 - "));
    Serial.print(fp.capacity() - 1);
    Serial.println(F("), a range \"first last\", or 9999 to wipe all:"));

    while (!Serial.available()) delay(1);
    uint16_t first = (uint16_t)Serial.parseInt();

    // Check for a second number (range mode)
    delay(50);  // give the rest of the line time to arrive
    uint16_t second = 0;
    bool hasSecond = false;
    if (Serial.available()) {
        int peeked = Serial.peek();
        if (peeked == ' ' || (peeked >= '0' && peeked <= '9')) {
            second   = (uint16_t)Serial.parseInt();
            hasSecond = (second != 0 || first != second);
        }
    }
    while (Serial.available()) Serial.read();  // flush rest of line

    // ── Wipe all ──────────────────────────────────────────────────────────────
    if (first == 9999) {
        Serial.println(F("WARNING: This will permanently erase ALL stored fingerprints!"));
        Serial.println(F("Type YES and press Enter to confirm, or anything else to cancel:"));
        String line = "";
        while (line.length() == 0) {
            while (!Serial.available()) delay(10);
            line = Serial.readStringUntil('\n');
            line.trim();
        }
        if (line == "YES") {
            if (fp.deleteAllFingerprints()) {
                Serial.println(F("[OK] All fingerprints deleted."));
                fp.ledFlash(FP_LED_RED, 3);
            } else {
                Serial.println(F("[FAIL] Wipe failed."));
            }
        } else {
            Serial.println(F("Cancelled."));
        }
        printCount();
        return;
    }

    // ── Range delete ──────────────────────────────────────────────────────────
    if (hasSecond) {
        uint16_t lo = min(first, second);
        uint16_t hi = max(first, second);
        if (hi >= fp.capacity()) {
            Serial.println(F("Last ID out of range."));
            return;
        }
        Serial.print(F("Deleting IDs "));
        Serial.print(lo); Serial.print(F(" – ")); Serial.println(hi);
        if (fp.deleteRange(lo, hi)) {
            Serial.print(F("[OK] Deleted IDs "));
            Serial.print(lo); Serial.print(F(" – ")); Serial.println(hi);
        } else {
            Serial.print(F("[FAIL] Delete range failed: 0x")); Serial.println(fp.lastCC, HEX);
        }
        printCount();
        return;
    }

    // ── Single delete ─────────────────────────────────────────────────────────
    if (first >= fp.capacity()) {
        Serial.println(F("ID out of range."));
        return;
    }
    if (!fp.templateExists(first)) {
        Serial.print(F("Slot ")); Serial.print(first); Serial.println(F(" is already empty."));
        return;
    }

    Serial.print(F("Deleting ID ")); Serial.println(first);
    if (fp.deleteFingerprint(first)) {
        Serial.print(F("[OK] Deleted ID ")); Serial.println(first);
    } else {
        Serial.print(F("[FAIL] Delete failed: 0x")); Serial.println(fp.lastCC, HEX);
    }
    printCount();
}
