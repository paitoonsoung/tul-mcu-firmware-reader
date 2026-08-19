# TUL MCU Firmware Reader Documentation

This directory is the project notebook: hardware facts, wiring, software architecture, test results, target-MCU notes, and build information belong here.

## Documents

| Document | Purpose |
|---|---|
| [architecture.md](architecture.md) | Overall software architecture and development milestones |
| [hardware.md](hardware.md) | Actual TUL prototype, connectors, power and hardware boundaries |
| [arduino-setup.md](arduino-setup.md) | Arduino IDE build environment and dependency record |
| [atmega328p-isp.md](atmega328p-isp.md) | First ATmega328P ISP proof-of-concept wiring and test |
| [supported-mcu.md](supported-mcu.md) | MCU support roadmap |
| [bench-log.md](bench-log.md) | Measured values, experiments and test results |

## Images

The `images/` directory contains reference diagrams that can be versioned with the project. The `photos/` directory is reserved for photographs of the actual prototype and target boards.

## Documentation rule

Separate **verified facts**, **measured values**, **design decisions**, and **future ideas**. If a value has not been measured or confirmed, mark it as pending instead of guessing.

Real photographs can be added later without changing the software structure. Keep original/reference images where possible and give each image a short description in the surrounding document.
