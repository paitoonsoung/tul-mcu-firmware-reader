# TUL MCU Firmware Reader — Hardware Photos

This page collects photographs of the actual hardware used during development and verification of the TUL MCU Firmware Reader.

The photographs are treated as physical-reference evidence for the prototype board, connectors, modules, target boards, wiring, and measurement points.

## Main Board

Photos of the complete instrument and the main board will be added here as they are verified.

- [Photo archive](photos/)
- Main board: PanLee ZX3D50CE08S-V16-USRC
- Wireless module: WT32-S3-WROVER (Wireless-Tag)
- MCU platform: ESP32-S3

## Connector Reference

### J3 — DEBUG

The J3 photographs should be used together with the pinout documented in [`hardware.md`](hardware.md). In particular, the physical board photo is the reference for verifying the connector orientation and the measured pin assignment.

Current verified J3 pinout:

| Pin | Signal |
|---:|---|
| 1 | 5V |
| 2 | 3V3 |
| 3 | UART_DEV(0) TxD |
| 4 | UART_DEV(0) RxD |
| 5 | RESET (EN) |
| 6 | BOOT (GPIO0) |
| 7 | GND |

### J5 — I/O

J5 photographs should be used together with the pinout documented in [`hardware.md`](hardware.md).

## Target and Test Hardware

Photo groups may include:

- ATmega328P-AU target board
- Target-side wiring
- ISP test setup
- Measurement points and measured voltages
- Successful read/program test setup

## Evidence Policy

- Keep photographs of the actual prototype separate from generic reference images.
- Do not replace earlier evidence when adding newer photographs.
- Use descriptive filenames and preserve the original board orientation where practical.
- When a photograph is used to verify a pinout or measurement, keep it in the repository as part of the hardware record.

See also [`docs/photos/README.md`](photos/README.md) for the photo archive and naming guidance.
