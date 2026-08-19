# TUL MCU Firmware Reader Architecture

## Purpose

Define the initial architecture for the standalone TUL MCU Firmware Reader & Programmer running on WT32-SC01 Plus (ESP32-S3).

## Design principles

- Keep hardware, transport, MCU protocol, programmer logic, UI, and storage separated.
- Start with one real target MCU and expand only after the first read/write path is proven.
- Keep the main application independent from a specific MCU family.
- Prefer non-blocking state-driven operations so the UI remains responsive during long reads, erases, writes, and verification.

## Logical data flow

```text
WT32-SC01 Plus UI
       |
       v
Application / State Machine
       |
       v
Programmer Engine
       |
       v
MCU Driver / Protocol
       |
       v
Transport Layer
       |
       v
Target MCU
       |
       v
Firmware Buffer
       |
       v
Storage (BIN / HEX)
```

## Initial modules

### Application
Owns startup, high-level state, user actions, and error reporting.

### UI
Owns display and touch interaction. It must not directly manipulate target MCU protocol signals.

### Programmer Engine
Provides operations such as detect, connect, read ID, read memory, erase, program, verify, reset, and disconnect.

### MCU Driver
Implements the protocol and memory model for one supported MCU family. New MCU families are added independently.

### Transport
Provides low-level physical communication such as UART, SPI, GPIO/bit-bang, or another interface required by a target MCU.

### Storage
Handles firmware images and metadata such as target identification, image size, checksum/CRC, and read/write results.

## First implementation boundary

The first firmware milestone should prove the WT32-SC01 Plus hardware path before adding a large programmer implementation:

1. ESP32-S3 boot and diagnostics.
2. Display initialization.
3. Touch initialization.
4. SD/storage initialization if used by the final hardware design.
5. Debug/serial diagnostics.
6. Only then select and implement the first real target MCU driver.

## Current status

The repository currently contains documentation and repository configuration only. No firmware implementation is assumed to exist yet.
