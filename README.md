# TUL MCU Firmware Reader

Standalone MCU Firmware Reader & Programmer using WT32-SC01 Plus (ESP32-S3).

## Current development stage

The project is in active hardware bring-up and firmware architecture development. The first real target is an ATmega328P-AU on a hand-built logic-probe/test instrument.

### MCU milestone

- ESP32-S3 firmware runs on the TUL prototype.
- J5 GPIO mapping is defined for the first bench test.
- AVR ISP programming-mode entry is implemented.
- ATmega328P signature read is implemented.
- No erase, Flash write, fuse write, or lock-bit write is implemented yet.

Expected first successful target signature:

```text
1E 95 0F
```

### Instrument milestone

- Proven TUL boot/logo screen.
- ST7796 parallel display and FT5x06 touch configuration retained.
- microSD diagnostics and read/write verification retained.
- Touch Utility menu added.
- Persistent display/WiFi/BLE settings added with ESP32 NVS.
- WiFi scan and saved-network credentials added.
- On-screen keyboard with SHIFT added.
- Local-only microSD format operation added with confirmation.
- Engineering/diagnostic menu added.
- Optional MQTT/Home Assistant discovery and telemetry layer added.

## Repository map

```text
.
├── README.md
├── LICENSE
├── docs/
│   ├── README.md
│   ├── architecture.md
│   ├── hardware.md
│   ├── arduino-setup.md
│   ├── atmega328p-isp.md
│   ├── supported-mcu.md
│   ├── bench-log.md
│   ├── utility-network.md
│   ├── images/
│   └── photos/
└── firmware/
    └── tul-mcu-reader/
        ├── tul-mcu-reader.ino
        ├── board_config.h
        ├── atmega328p_isp.h/.cpp
        ├── storage_test.h/.cpp
        ├── storage_manager.h/.cpp
        ├── settings.h/.cpp
        ├── network_manager.h/.cpp
        ├── keyboard.h
        ├── ui_manager.h
        ├── ha_config.h
        └── ha_integration.h/.cpp
```

## Documentation

Start with [docs/README.md](docs/README.md).

Important references:

- [Hardware](docs/hardware.md)
- [Arduino IDE setup](docs/arduino-setup.md)
- [ATmega328P ISP wiring](docs/atmega328p-isp.md)
- [Architecture](docs/architecture.md)
- [MCU roadmap](docs/supported-mcu.md)
- [Utility / Network](docs/utility-network.md)
- [Bench log](docs/bench-log.md)

## Home Assistant boundary

The reader can publish telemetry through MQTT when `PubSubClient` is installed and `ha_config.h` contains the broker used by the existing TUL Home Assistant system. Remote SD formatting is intentionally not exposed.

## Project rule

This repository is also the engineering notebook for the instrument. Hardware measurements, wiring, test results, design decisions, firmware revisions, known problems, and future plans should be recorded here so the project can be reconstructed later without relying on chat history or a single development computer.

Real photographs can be added later under `docs/photos/`. Reference diagrams are kept separately under `docs/images/`.
