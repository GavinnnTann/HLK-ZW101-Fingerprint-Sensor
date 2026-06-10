/*
 * HLK-ZW Fingerprint Sensor Library
 * Example: Read and display module system parameters
 *
 * Reads module system parameters via ReadSysPara (0x0F) and prints a summary
 * to the Serial monitor. All parameters persist in module flash and survive
 * power cycles. Use the Settings tab in the Python tester to change them.
 *
 * Press any key in the Serial monitor to re-read parameters.
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

// ── Print system info ─────────────────────────────────────────────────────────

void printSystemInfo() {
    uint16_t capacity = 0;
    uint8_t  secLevel = 0;
    uint8_t  pktIdx   = 0;
    uint8_t  baudN    = 0;

    if (!fp.readSysParam(&capacity, &secLevel, &pktIdx, &baudN)) {
        Serial.println(F("ERROR: Failed to read system parameters."));
        return;
    }

    const uint16_t pktSizes[4] = { 32, 64, 128, 256 };

    Serial.println(F("\n--- Module System Parameters ---"));

    // Capacity and likely family
    Serial.print(F("  Template capacity : "));
    Serial.print(capacity); Serial.println(F(" slots"));
    Serial.print(F("  Likely HLK family : "));
    if      (capacity == 50)  Serial.println(F("ZW101 / ZW06xx / ZW09xx"));
    else if (capacity == 100) Serial.println(F("ZW111 / ZW06xx / ZW09xx / ZW30xx"));
    else                      Serial.println(F("EF-01 compatible (unknown variant)"));

    // Enrolled count
    Serial.print(F("  Enrolled templates: "));
    Serial.print(fp.getTemplateCount());
    Serial.print(F(" / "));
    Serial.println(capacity);

    // Communication settings
    Serial.print(F("  Baud rate         : "));
    Serial.print((uint32_t)baudN * 9600);
    Serial.println(F(" bps"));

    Serial.print(F("  Packet size       : "));
    Serial.print(pktIdx < 4 ? pktSizes[pktIdx] : 0);
    Serial.println(F(" bytes"));

    // Security level
    Serial.print(F("  Security level    : "));
    Serial.print(secLevel);
    Serial.println(F("  (1 = most permissive, 5 = strictest)"));

    Serial.println(F("--------------------------------"));
    Serial.println(F("Press any key to refresh."));
}

// ── Setup / Loop ──────────────────────────────────────────────────────────────

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);
    Serial.println(F("\nHLK-ZW System Info"));
    Serial.println(F("==================="));

    if (CTRL >= 0) {
        pinMode(CTRL, OUTPUT);
        digitalWrite(CTRL, HIGH);  // assert HIGH to enable low-power circuit
    }

    if (!fp.begin()) {
        Serial.println(F("ERROR: Module not found. Check wiring and baud rate."));
        while (true) delay(1000);
    }

    Serial.println(F("Module ready."));
    printSystemInfo();
}

void loop() {
    if (Serial.available()) {
        while (Serial.available()) Serial.read();
        printSystemInfo();
    }
    delay(50);
}
