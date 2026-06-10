/*
 * HLK-ZW Fingerprint Sensor Library
 * Example: LED effects demo
 *
 * Cycles through every available LED function and colour, pausing 2 seconds
 * between each. Progress is printed to the Serial monitor.
 *
 * RGB effects (AURALEDCONFIG 0x3C) are supported on ZW1xx modules.
 * On passive-LED variants (ZW06xx, ZW09xx, ZW30xx) the library automatically
 * falls back to simple ledOn() / ledOff() — you will see "On/Off" in the log
 * instead of specific effect names, but the code runs without modification.
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

// ── Helper: run one effect, print result, wait ────────────────────────────────

void show(const __FlashStringHelper *label, bool ok, uint32_t holdMs = 2000) {
    Serial.print(F("  ")); Serial.print(label);
    Serial.println(ok ? F(""): F("  (fallback to simple on/off)"));
    delay(holdMs);
}

// ── Setup / Loop ──────────────────────────────────────────────────────────────

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);
    Serial.println(F("\nHLK-ZW LED Effects Demo"));
    Serial.println(F("========================"));

    if (CTRL >= 0) {
        pinMode(CTRL, OUTPUT);
        digitalWrite(CTRL, HIGH);  // assert HIGH to enable low-power circuit
    }

    if (!fp.begin()) {
        Serial.println(F("ERROR: Module not found. Check wiring and baud rate."));
        while (true) delay(1000);
    }

    Serial.println(F("Module ready. Starting LED demo...\n"));
}

void loop() {
    // ── Breathing ────────────────────────────────────────────────────────────
    Serial.println(F("[ Breathing ]"));
    show(F("Blue"),   fp.ledBreathing(FP_LED_BLUE));
    show(F("Green"),  fp.ledBreathing(FP_LED_GREEN));
    show(F("Red"),    fp.ledBreathing(FP_LED_RED));
    show(F("Cyan"),   fp.ledBreathing(FP_LED_CYAN));
    show(F("Purple"), fp.ledBreathing(FP_LED_PURPLE));
    show(F("Yellow"), fp.ledBreathing(FP_LED_YELLOW));
    show(F("White"),  fp.ledBreathing(FP_LED_WHITE));

    // ── Flash ─────────────────────────────────────────────────────────────────
    Serial.println(F("\n[ Flash  x3 ]"));
    show(F("Blue"),   fp.ledFlash(FP_LED_BLUE,   3));
    show(F("Green"),  fp.ledFlash(FP_LED_GREEN,  3));
    show(F("Red"),    fp.ledFlash(FP_LED_RED,    3));
    show(F("White"),  fp.ledFlash(FP_LED_WHITE,  3));

    // ── Steady on ─────────────────────────────────────────────────────────────
    Serial.println(F("\n[ Steady On ]"));
    show(F("Blue"),   fp.ledSteady(FP_LED_BLUE));
    show(F("Green"),  fp.ledSteady(FP_LED_GREEN));
    show(F("Red"),    fp.ledSteady(FP_LED_RED));
    show(F("White"),  fp.ledSteady(FP_LED_WHITE));
    show(F("Cyan"),   fp.ledSteady(FP_LED_CYAN));
    show(F("Purple"), fp.ledSteady(FP_LED_PURPLE));

    // ── Gradual open / close ──────────────────────────────────────────────────
    Serial.println(F("\n[ Gradual ]"));
    show(F("White  — open"),  fp.ledGradOpen(FP_LED_WHITE));
    show(F("White  — close"), fp.ledGradClose(FP_LED_WHITE));
    show(F("Blue   — open"),  fp.ledGradOpen(FP_LED_BLUE));
    show(F("Blue   — close"), fp.ledGradClose(FP_LED_BLUE));

    // ── Off ───────────────────────────────────────────────────────────────────
    Serial.println(F("\n[ Off ]"));
    show(F("LED off"), fp.ledOff(), 1000);

    Serial.println(F("\n-- loop --\n"));
    delay(1000);
}
