/*
 * HLK-ZW Fingerprint Sensor Library
 * Example: Print a visual storage map of all template slots
 *
 * Reads the slot occupancy bitmap (READ_INDEX 0x1F) and prints a grid to the
 * Serial monitor. [##] = occupied, [  ] = empty. The map auto-refreshes every
 * 5 seconds, or immediately when any key is pressed.
 *
 * Works with any capacity — the grid width adjusts automatically to capacity.
 * Falls back to TEMPLATECOUNT if READ_INDEX is not supported by the firmware.
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
#define COLS   10   // template slots per row in the printed grid
constexpr int CTRL = -1;  // set to GPIO number to enable low-power circuit; -1 = disabled

FingerprintModule fp(Serial1, FP_RX, FP_TX);

// Fixed-size buffer large enough for all current EF-01 variants (max 256 slots).
bool states[256];

// ── Print the map ─────────────────────────────────────────────────────────────

void printMap() {
    uint16_t cap = fp.capacity();

    if (!fp.getStorageMap(states, cap)) {
        // READ_INDEX not supported — fall back to count only
        Serial.println(F("READ_INDEX not supported by this firmware."));
        Serial.print(F("Template count: "));
        Serial.println(fp.getTemplateCount());
        return;
    }

    uint16_t enrolled = 0;
    for (uint16_t i = 0; i < cap; i++) if (states[i]) enrolled++;

    Serial.println();
    Serial.print(F("--- Storage Map  ("));
    Serial.print(enrolled);
    Serial.print(F(" / "));
    Serial.print(cap);
    Serial.println(F(" slots occupied) ---"));

    for (uint16_t i = 0; i < cap; i++) {
        // Row header
        if (i % COLS == 0) {
            if (i > 0) Serial.println();
            if      (i < 10)  Serial.print(F("  "));
            else if (i < 100) Serial.print(' ');
            Serial.print(i);
            Serial.print(F(": "));
        }
        Serial.print(states[i] ? F("[##]") : F("[  ]"));
    }
    Serial.println();
    Serial.println(F("--- [##]=occupied  [  ]=empty ---"));
}

// ── Setup / Loop ──────────────────────────────────────────────────────────────

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);
    Serial.println(F("\nHLK-ZW Storage Map"));
    Serial.println(F("==================="));

    if (CTRL >= 0) {
        pinMode(CTRL, OUTPUT);
        digitalWrite(CTRL, HIGH);  // assert HIGH to enable low-power circuit
    }

    if (!fp.begin()) {
        Serial.println(F("ERROR: Module not found. Check wiring and baud rate."));
        while (true) delay(1000);
    }

    Serial.print(F("Module ready  |  Capacity: "));
    Serial.println(fp.capacity());
    Serial.println(F("Press any key to refresh."));

    printMap();
}

void loop() {
    // Refresh on keypress, or every 5 seconds
    unsigned long t = millis();
    while (millis() - t < 5000) {
        if (Serial.available()) { while (Serial.available()) Serial.read(); break; }
        delay(50);
    }
    printMap();
}
