# TUL MCU Firmware Reader Hardware

## Target platform

- Main controller: WT32-SC01 Plus
- MCU: ESP32-S3
- User interface: onboard display/touch hardware of the WT32-SC01 Plus

## Hardware boundary

The programmer hardware must provide a controlled interface between the ESP32-S3 and the target MCU. Exact target signal assignments are not defined yet and must be verified against the actual hardware before implementation.

Candidate signal groups include:

- Target VCC
- Target GND
- Target RESET
- Target TX/RX
- Target clock/data signals where required by the selected programming protocol

## Electrical requirements

- Confirm target voltage before driving any target signal.
- Avoid assuming that every target MCU is 3.3 V tolerant.
- Add level shifting or voltage control where required.
- Define a safe target-power policy before implementing programming operations.

## Bring-up order

1. Verify WT32-SC01 Plus power and ESP32-S3 boot.
2. Verify display.
3. Verify touch input.
4. Verify storage if used.
5. Verify debug/serial output.
6. Define and validate the first target MCU interface.
7. Implement the first transport and MCU driver only after the electrical interface is confirmed.

## Current status

No target-MCU pin map or programmer connector assignment is committed yet. Do not invent pin numbers until the actual WT32-SC01 Plus hardware and intended programmer interface are verified.
