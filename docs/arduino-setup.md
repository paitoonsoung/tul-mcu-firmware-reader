# Arduino IDE Setup

This document records the build environment for the current firmware.

## Target board

- Hardware: PanLee ZX3D50CE08S-V16-USRC
- Wireless module: WT32-S3-WROVER
- MCU: ESP32-S3
- Arduino board profile: **ESP32S3 Dev Module**

## Current firmware

Open:

```text
firmware/tul-mcu-reader/tul-mcu-reader.ino
```

## Libraries

The current firmware uses:

- ESP32 Arduino core APIs: `WiFi`, `Preferences`, `SD`, `SPI`, BLE.
- **LovyanGFX** for the ST7796 parallel display and FT5x06 touch controller.
- **PubSubClient** is optional. Install it to enable MQTT/Home Assistant telemetry and discovery.

Keep dependency versions recorded from the physical development PC before declaring a build reproducible.

## Serial Monitor

```text
Baud rate: 115200
Line ending: any
```

Expected startup diagnostics include:

```text
TUL MCU Firmware Reader
Utility / Engineering / Network framework enabled
ATmega328P signature: 1E 95 0F
TARGET OK
```

## Board options

The exact Arduino IDE option set must be recorded from the physical development PC before the first reproducible build is declared. Do not silently guess a board option and treat it as verified hardware information.

| Option | Current record | Status |
|---|---|---|
| Board | ESP32S3 Dev Module | Confirmed project target |
| USB CDC On Boot | To be recorded | Pending bench verification |
| USB Mode | To be recorded | Pending bench verification |
| Flash Mode | To be recorded | Pending bench verification |
| Flash Size | To be recorded | Pending bench verification |
| Partition Scheme | To be recorded | Pending bench verification |
| PSRAM | To be recorded | Pending bench verification |
| Upload Speed | To be recorded | Pending bench verification |
| Core version | To be recorded | Pending bench verification |
| LovyanGFX version | To be recorded | Pending bench verification |
| PubSubClient version | To be recorded | Optional / pending bench verification |

## Important safety boundary

The local SD format operation is intentionally not exposed through Home Assistant. Target MCU erase/write is also still disabled until the ATmega328P write path is independently bench-tested.

## Build reproducibility

When the next successful physical test is completed, update this file with the exact Arduino IDE settings, ESP32 Arduino core version, LovyanGFX version and optional PubSubClient version used for that build.
