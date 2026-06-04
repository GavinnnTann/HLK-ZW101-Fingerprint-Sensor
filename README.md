# HLK-ZW101 Tester Program

A desktop tool for testing and managing the HLK-ZW101 capacitive fingerprint sensor over UART.

---

## Where to Buy

| Item | Link |
|------|------|
| HLK-ZW101 Fingerprint Sensor + CH340 Adapter | https://www.aliexpress.com/item/1005011644712935.html?spm=a2g0o.order_list.order_list_main.23.417f18020u6hpK |

---

## Circuit

<!-- Insert wiring photo here -->

---

## Wiring

Connect the module's wires to your USB-serial adapter as follows.

> **Note:** TX and RX are labelled from the adapter's perspective.
> Black (module) → adapter TX means the adapter transmits to the module.

| HL-ZW101 Wire | Adapter Pin |
|------|-------------|
| 🔴 Red (GND) | GND |
| ⚫ Black (RX) | TX |
| 🟡 Yellow (TX) | RX |
| 🟢 Green (VCC)| 3V3 |
| ⚪ White (V_SENSOR)| 3V3 |
| 🔵 Blue (TOUCH_OUT) | NC (leave unconnected) |

---

## Requirements

- Python 3.10+
- Windows / macOS / Linux
- A USB-serial adapter (CH340, CP2102, or FTDI recommended for auto-detection)

---

## Installation

```bash
pip install -r requirements.txt
python '.\HL-ZW101 Tester Program.py' 
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
