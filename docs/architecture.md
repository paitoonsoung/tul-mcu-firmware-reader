# TUL MCU Firmware Reader Architecture

## Purpose

Define the initial architecture for the standalone TUL MCU Firmware Reader & Programmer running on the actual PanLee ZX3D50CE08S-V16-USRC / WT32-S3-WROVER (ESP32-S3) prototype.

## Design principles

- Keep hardware, transport, MCU protocol, programmer logic, UI, and storage separated.
- Start with one real target MCU and expand only after the first read/write path is proven.
- Keep the main application independent from a specific MCU family.
- Prefer non-blocking state-driven operations so the UI remains responsive during long reads, erases, writes, and verification.
- Never assume target voltage or signal direction; the electrical interface must be verified first.

## Logical data flow

```text
WT32-S3-WROVER UI
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
Provides low-level communication such as UART, SPI, GPIO/bit-bang, or another interface required by a target MCU.

### Storage
Handles firmware images and metadata such as target identification, image size, checksum/CRC, and read/write results.

## First real target: ATmega328P

The first physical target selected for proof-of-concept is an ATmega328P-AU on a hand-built logic-probe/test instrument supplied for bench testing.

The first driver uses the ATmega328P serial programming (ISP) interface and initially implements only the safe identification step:

```text
ESP32-S3
   |
   | SPI ISP
   v
ATmega328P
   |
   v
Read Signature
```

Expected ATmega328P signature:

```text
1E 95 0F
```

The signature must be read successfully before implementing Flash read, erase, program, or verification operations.

## First firmware milestone

1. ESP32-S3 serial diagnostics.
2. Configure the J5 GPIO resources for the ISP proof-of-concept.
3. Enter ATmega328P programming mode.
4. Read and validate the device signature.
5. Report success/failure without writing target memory.
6. Only after bench validation, implement Flash read and file storage.

## Current status

Repository foundation and hardware documentation exist. The first real target is now ATmega328P-AU and the initial ISP identification driver is being introduced. Target voltage/level-shifting hardware must be validated before physical connection.
