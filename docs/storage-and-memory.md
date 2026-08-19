# TUL Storage and Memory Architecture

## Purpose

Define how the TUL MCU Firmware Reader handles firmware images, BIOS files, photographs, logs, and working memory without requiring large firmware images to fit in ESP32-S3 RAM.

## Verified board specification

The product specification supplied for the purchased WT32-SC01 Plus states:

- MCU: ESP32-S3 dual-core 240 MHz
- Display: 3.5-inch IPS 480x320
- Touch: capacitive multi-touch
- USB: Type-C
- Flash: 16 MB
- PSRAM: 8 MB
- Onboard microSD connector

These values are recorded as the purchased-board specification. Actual runtime memory availability will be measured later on the assembled prototype.

## Core storage rule

The ESP32-S3 is the controller and working engine. It is not the long-term repository for the firmware collection.

Large firmware images must not be loaded into RAM as one complete buffer.

Use streaming/chunked transfer:

```text
microSD
   |
   | read chunk
   v
ESP32-S3 buffer
   |
   | protocol transfer
   v
Target MCU
   |
   | next chunk
   +-------------------->
```

A firmware image may therefore be much larger than available RAM.

## Memory roles

### ESP32-S3 internal RAM

Use for:

- real-time protocol state
- small command/status buffers
- UI state
- control logic
- critical data that must remain fast

### PSRAM

Use as optional working memory for:

- display/image buffers
- larger temporary buffers
- protocol staging where useful
- non-critical temporary data

Do not design the programmer around storing a complete BIOS image in PSRAM.

### Internal Flash

The 16 MB board Flash is reserved primarily for the TUL application, firmware resources, configuration, and other fixed assets required for standalone operation.

Do not use internal Flash as the main firmware archive.

### microSD

microSD is the long-term removable storage layer for:

- `.bin` firmware images
- `.hex` files
- EEPROM dumps
- BIOS images
- MCU identification data
- target records
- checksums and verification results
- service notes
- photographs and wiring references
- diagnostic logs

The practical maximum card size/filesystem combination will be validated on the actual board before documenting a guaranteed capacity.

## Proposed SD directory model

```text
/TUL/
├── database/
│   ├── mcu/
│   ├── flash/
│   ├── eeprom/
│   └── protocols/
├── firmware/
│   ├── atmega/
│   ├── stm32/
│   ├── pic/
│   └── stc/
├── bios/
│   ├── notebook/
│   └── industrial/
├── targets/
│   └── <target-id>/
│       ├── board.jpg
│       ├── wiring.jpg
│       ├── notes.txt
│       └── firmware/
├── logs/
└── backups/
```

This is a design proposal, not yet a required filesystem layout for the first firmware build.

## Firmware transfer policy

All future read/write/program operations should support a streaming API conceptually similar to:

```text
open image
  -> read chunk
  -> process/verify chunk
  -> transfer chunk
  -> update progress/checksum
  -> repeat
  -> close image
```

The exact chunk size will be selected after measuring available RAM, SD performance, display workload, and the requirements of each target protocol.

## Integrity

For every saved firmware image, the system should eventually record at least:

- target manufacturer
- target device/signature
- interface/protocol
- image size
- checksum/CRC or stronger digest where appropriate
- read/program timestamp
- operation result

A future implementation may use a metadata sidecar file or a database on SD rather than keeping a large index in RAM.

## Safety rule

A target image must never be selected for programming solely because its filename looks correct. The future programmer flow should compare target identity, image metadata, image size, and other applicable constraints before enabling a write operation.

## Validation plan

Before relying on SD storage for large firmware collections, measure on the actual WT32-SC01 Plus prototype:

1. SD card initialization.
2. Filesystem type and capacity reporting.
3. Large-file sequential read/write.
4. Streaming read with small buffers.
5. Available internal RAM during UI operation.
6. Available PSRAM during UI operation.
7. Sustained firmware transfer performance.
8. Recovery behavior after an interrupted transfer.

## Design conclusion

A multi-gigabyte firmware/BIOS collection is a storage problem, not an ESP32 RAM problem, provided the implementation streams files from microSD instead of loading complete images into memory.
