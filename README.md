# HLK-ZW Fingerprint Sensor

[![Download](https://img.shields.io/github/v/release/GavinnnTann/HLK-ZW-Fingerprint-Sensor?label=Download%20Tester&style=for-the-badge)](https://github.com/GavinnnTann/HLK-ZW-Fingerprint-Sensor/releases/download/v1.1.0/HLK-ZW.Tester.Program.exe)

An Arduino library and desktop tester for the HLK-ZW series capacitive fingerprint sensors (EF-01 UART protocol), manufactured by **Shenzhen Hi-Link Electronic Co., Ltd**. Supports HLK-ZW101, ZW111, ZW06xx, ZW09xx, ZW30xx, and other AS608/R307-compatible modules.

The original Hi-Link demo was in Chinese and lacked LED control. This project provides a complete Arduino driver with six example sketches, plus a Python-based GUI tester for exploring the sensor over USB without writing any firmware.

<img src="extras/Images/HL-ZW101%20Product.png" width="450" alt="Product Screenshot">
<img src="extras/Images/Program%20screenshot.png" width="450" alt="Program Screenshot">

---

## Where to Buy

| Item | Link |
|------|------|
| HLK-ZW101 Fingerprint Sensor + CH340 Adapter | https://www.aliexpress.com/item/1005011644712935.html?spm=a2g0o.order_list.order_list_main.23.417f18020u6hpK |

---

## Arduino Library

### Installation

**Manual:** Download or clone this repo and copy `HLK_fingerprint.h` and `HLK_fingerprint.cpp` into your Arduino libraries folder, or place them alongside your sketch.

**Arduino IDE (zip install):** Sketch → Include Library → Add .ZIP Library… → select the downloaded zip.

### Quick Start

```cpp
#include <HLK_fingerprint.h>

FingerprintModule fp(Serial1, /*RX=*/16, /*TX=*/17);

void setup() {
    fp.begin();           // verifies password, reads capacity
}

void loop() {
    uint16_t score;
    int16_t id = fp.matchFingerprint(score);  // blocks up to 10 s
    if (id >= 0)  Serial.printf("Match: ID %d  score %d\n", id, score);
    else          Serial.println("No match");
}
```

### Supported Modules

| Module | Template Slots | RGB LED |
|--------|---------------|---------|
| HLK-ZW101 | 50 | Yes (AURALEDCONFIG) |
| HLK-ZW111 | 100 | Yes (AURALEDCONFIG) |
| HLK-ZW06xx | 50 or 100 | No (simple on/off) |
| HLK-ZW09xx | 50 or 100 | No (simple on/off) |
| HLK-ZW30xx | 100 | No (simple on/off) |
| AS608 / R307 compatible | varies | depends on firmware |

LED convenience wrappers (`ledBreathing`, `ledFlash`, `ledSteady`, etc.) automatically fall back to simple on/off for passive-LED variants — no code changes needed.

### Key API

```cpp
// Init
bool begin(uint32_t baud = 57600);

// Enrollment & matching
int16_t enrollFingerprint(uint16_t id = 0xFFFF); // 0xFFFF = auto-assign
int16_t matchFingerprint(uint16_t &score);

// Template management
bool deleteFingerprint(uint16_t id);
bool deleteRange(uint16_t first, uint16_t last);
bool deleteAllFingerprints();
bool getStorageMap(bool *states, uint16_t maxSlots);

// LED (auto-fallback for passive variants)
bool ledBreathing(uint8_t color = FP_LED_WHITE);
bool ledFlash(uint8_t color, uint8_t cycles = 3);
bool ledSteady(uint8_t color = FP_LED_WHITE);
bool ledOff();

// System settings
bool readSysParam(uint16_t *capacity, uint8_t *secLevel, uint8_t *pktIdx, uint8_t *baudN);
bool setSecurityLevel(uint8_t level);  // 1–5
```

### Examples

| Sketch | Description |
|--------|-------------|
| `enroll` | Low-level two-scan enrollment with serial prompt for ID |
| `fingerprint` | High-level 1:N match with LED feedback |
| `delete_fingerprint` | Single delete, range delete, or full wipe |
| `storage_map` | ASCII grid of all template slots, auto-refreshes every 5 s |
| `led_effects` | Cycles all LED functions and colours |
| `system_info` | Reads and prints module system parameters |
| `MCU_Adapter` | ESP32 as USB-CDC ↔ UART bridge for use with the Python tester |

All examples include an optional **CTRL pin** for low-power circuit designs — set `constexpr int CTRL = -1` (default, disabled) to a GPIO number to enable.

---

## Python Tester (Windows GUI)

A standalone desktop tool for testing and managing the sensor over USB without writing firmware. Works with the CH340 adapter or an ESP32 running the `MCU_Adapter` sketch.

### Circuit

**FP Sensor (MX1.0-6P) → CH340 (Jumper)**

<img src="extras/Images/Circuit.png" width="450" alt="Setup">

### Wiring

Connect the module's wires to your USB-serial adapter as follows. Microcontrollers like ESP32, Arduino, STM32, Pico, and Raspberry Pi can also be used in place of the CH340 adapter (see `MCU_Adapter` example).

> **Note:** TX and RX are labelled from the adapter's perspective.
> Black (module) → adapter TX means the adapter transmits to the module.
> Colour scheme is based on the Hi-Link distributor above; colours may vary between vendors.
>
> **TOUCH_OUT (Blue):** Leave unconnected for USB testing.
> For embedded use, connect to a GPIO — the sensor asserts this HIGH when a finger is detected,
> making it suitable for interrupt-driven wakeup from deep sleep on ESP32/STM32.
> Low power design is available — refer to the [datasheet](extras/HLK-ZW101%20Datasheet.pdf).

| HLK-ZW101 Wire | Adapter Pin |
|------|-------------|
| 🔴 Red (GND) | GND |
| ⚫ Black (RX) | TX |
| 🟡 Yellow (TX) | RX |
| 🟢 Green (VCC) | 3V3 |
| 🔵 Blue (TOUCH_OUT) | NC (leave unconnected for USB testing) |
| ⚪ White (V_SENSOR) | 3V3 |

### Requirements (source)

- Python 3.10+
- Windows / macOS / Linux
- A USB-serial adapter (CH340, CP2102, or FTDI recommended for auto-detection)

### Installation (source)

```bash
pip install -r requirements.txt
python HLK_ZW_Tester_Program.py
```

### Quick Start

1. Plug in your USB-serial adapter with the sensor wired up
2. Click **Refresh** — the correct COM port is usually selected automatically
3. Set the baud rate to **57600** (default)
4. Click **Connect** — the tool will automatically verify the module and load the storage map
5. Use **Enrollment** to register a fingerprint, then **Match** under Verification to test it

### Features

- **Auto-connect query** — verifies password, reads system parameters, and loads the storage map on every connection
- **Storage map** — visual grid showing all template slots at a glance
- **Enrollment** — two-scan enroll with progress feedback; auto-selects next free slot
- **Verification** — 1:N match with adjustable timeout and confidence score
- **Template management** — check, delete single, delete range, wipe all
- **LED control** — all 6 modes (Breathing, Flash, Steady On, Gradually Open, Gradually Close, Off); falls back to simple on/off for passive-LED variants
- **Settings** — security level, baud rate, packet size, password change

---

## Acknowledgements

This project is heavily inspired by the [Adafruit Fingerprint Sensor Library](https://github.com/adafruit/Adafruit-Fingerprint-Sensor-Library) by Adafruit Industries, which pioneered an accessible Arduino API for EF-01 UART fingerprint modules. The packet framing, confirm-code handling, and overall driver architecture in `HLK_fingerprint.cpp` follow the same conventions established by their library. Credit to Adafruit and the contributors of that project for laying the groundwork.
