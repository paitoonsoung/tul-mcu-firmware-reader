# Supported MCU Roadmap

No target MCU is marked as supported yet.

## Selection criteria

The first target should be an MCU that is physically available for bench testing and whose programming interface, voltage requirements, memory map, and read/write protocol can be verified directly.

## First-target workflow

1. Identify the exact MCU part number.
2. Confirm programming interface and entry sequence.
3. Confirm target voltage and signal levels.
4. Read device ID or equivalent identification data.
5. Read the complete firmware memory.
6. Save a binary image.
7. Calculate and record image integrity information.
8. Verify the saved image against a second read.
9. Only after read/verify is stable, implement erase/program.

## Status table

| MCU family | Interface | Read | Write | Verify | Status |
|---|---|---:|---:|---:|---|
| TBD | TBD | - | - | - | Candidate not selected |

## Rule

Do not add an MCU driver to the firmware tree until the exact target device and physical interface have been confirmed.
