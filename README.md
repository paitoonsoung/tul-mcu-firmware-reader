# TUL MCU Firmware Reader

Standalone MCU Firmware Reader & Programmer using WT32-SC01 Plus (ESP32-S3).

## Current development stage

The project is in the first hardware bring-up stage. The first real target is an ATmega328P-AU on a hand-built logic-probe/test instrument.

Current firmware milestone:

- ESP32-S3 firmware runs on the TUL prototype.
- J5 GPIO mapping is defined for the first bench test.
- AVR ISP programming-mode entry is implemented.
- ATmega328P signature read is implemented.
- No erase, Flash write, fuse write, or lock-bit write is implemented.

Expected first successful target signature:

```text
1E 95 0F
```

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
│   ├── images/
│   └── photos/
└── firmware/
    └── tul-mcu-reader/
        ├── tul-mcu-reader.ino
        ├── board_config.h
        ├── atmega328p_isp.h
        └── atmega328p_isp.cpp
```

## Documentation

Start with [docs/README.md](docs/README.md).

Important references:

- [Hardware](docs/hardware.md)
- [Arduino IDE setup](docs/arduino-setup.md)
- [ATmega328P ISP wiring](docs/atmega328p-isp.md)
- [Architecture](docs/architecture.md)
- [MCU roadmap](docs/supported-mcu.md)
- [Bench log](docs/bench-log.md)

## Project rule

This repository is also the engineering notebook for the instrument. Hardware measurements, wiring, test results, design decisions, firmware revisions, known problems, and future plans should be recorded here so the project can be reconstructed later without relying on chat history or a single development computer.

Real photographs can be added later under `docs/photos/`. Reference diagrams are kept separately under `docs/images/`.
