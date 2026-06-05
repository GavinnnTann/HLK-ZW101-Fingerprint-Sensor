# HLK-ZW101 Tester Program

[![Download](https://img.shields.io/github/v/release/GavinnnTann/HLK-ZW101-Tester?label=Download&style=for-the-badge)](https://github.com/GavinnnTann/HLK-ZW101-Tester/releases/latest/download/HL-ZW101.Tester.Program.exe)

A desktop tool for testing and managing the HLK-ZW101 capacitive fingerprint sensor over UART, manufactured by **Shenzhen Hi-Link Electronic Co., Ltd**. Demo provided by Hi-Link was written in chinese, and was without LED control.

Built on top of the [Adafruit Fingerprint Sensor Library](https://github.com/adafruit/Adafruit-Fingerprint-Sensor-Library) protocol, adapted and extended with additional controls for easier exploration of the device's communication protocol. Designed to make it straightforward to understand how the sensor works, and can be readily adapted into other microcontroller projects or programs.

<img src="Images/HL-ZW101%20Product.png" width="450" alt="Product Screenshot">
<img src="Images/Program%20screenshot.png" width="450" alt="Program Screenshot">

---

## Where to Buy

| Item | Link |
|------|------|
| HLK-ZW101 Fingerprint Sensor + CH340 Adapter | https://www.aliexpress.com/item/1005011644712935.html?spm=a2g0o.order_list.order_list_main.23.417f18020u6hpK |

---

## Circuit

**FP Sensor (MX1.0-6P) → CH340 (Jumper)**

<img src="Images/Circuit.png" width="450" alt="Setup">

---


## Wiring

Connect the module's wires to your USB-serial adapter as follows.

> **Note:** TX and RX are labelled from the adapter's perspective.
> Black (module) → adapter TX means the adapter transmits to the module.
> Colour scheme is based on the Hi-Link distributor above, colour may vary between different vendors.
>
> **TOUCH_OUT (Blue):** Leave unconnected for USB testing.
> For embedded use, connect to a GPIO pin — the sensor asserts this HIGH when a finger is detected,
> making it suitable for interrupt-driven wakeup from deep sleep on ESP32 / STM32.
> See the datasheet for electrical specs.

| HLK-ZW101 Wire | Adapter Pin |
|------|-------------|
| 🔴 Red (GND) | GND |
| ⚫ Black (RX) | TX |
| 🟡 Yellow (TX) | RX |
| 🟢 Green (VCC)| 3V3 |
| 🔵 Blue (TOUCH_OUT) | NC (leave unconnected for USB testing) |
| ⚪ White (V_SENSOR)| 3V3 |


---

## Requirements

- Python 3.10+
- Windows / macOS / Linux
- A USB-serial adapter (CH340, CP2102, or FTDI recommended for auto-detection)

---

## Installation

```bash
pip install -r requirements.txt
python '.\HLK_ZW101_Tester_Program.py' 
```

---

## Quick Start

1. Plug in your USB-serial adapter with the sensor wired up
2. Click **Refresh** — the correct COM port is usually selected automatically
3. Set the baud rate to **57600** (default)
4. Click **Connect** — the tool will automatically verify the module and load the storage map
5. Use **Enrollment** to register a fingerprint, then **Match** under Verification to test it

---

## Features

- **Auto-connect query** — verifies password, reads system parameters, and loads the storage map on every connection
- **Storage map** — visual grid showing all 50 template slots at a glance
- **Enrollment** — two-scan enroll with progress feedback; auto-selects next free slot
- **Verification** — 1:N match with adjustable timeout and confidence score
- **Template management** — check, delete single, delete range, wipe all
- **LED control** — all 6 modes (Breathing, Flash, Steady On, Gradually Open, Gradually Close, Off)
- **Settings** — security level, baud rate, packet size, password change
