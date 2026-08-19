# Arduino IDE Setup

This document records the build environment for the current firmware proof-of-concept.

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

The sketch currently uses only the Arduino core plus project-local source files:

```cpp
#include <Arduino.h>
#include "board_config.h"
#include "atmega328p_isp.h"
```

No third-party Arduino library is required by the current ISP proof-of-concept.

## Serial Monitor

```text
Baud rate: 115200
Line ending: any
```

Expected diagnostic output after a successful target connection:

```text
TUL MCU Firmware Reader
ATmega328P ISP Proof of Concept
No erase/write operations
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

## Dependency policy

Keep the proof-of-concept dependency-free where possible. If an external library is introduced later, record:

1. Library name.
2. Version.
3. Why it is required.
4. Where it is used.
5. Whether it is mandatory or optional.

## Build reproducibility

When the first successful physical test is completed, update this file with the exact Arduino IDE settings and ESP32 Arduino core version used for that successful build. This prevents the project from depending on undocumented local settings.
