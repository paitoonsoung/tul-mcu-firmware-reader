# Supported MCU Roadmap

## First target: ATmega328P

The first physical target selected for development is the ATmega328P-AU in the hand-built logic-probe/test instrument supplied for bench testing.

### Interface

The initial programming interface is AVR serial programming (ISP).

Required target signals:

- MOSI
- MISO
- SCK
- RESET
- VCC
- GND

### ATmega328P TQFP-32 reference pins

| Function | Port | TQFP-32 pin |
|---|---|---:|
| MOSI | PB3 | 15 |
| MISO | PB4 | 16 |
| SCK | PB5 | 17 |
| RESET | PC6 | 29 |
| VCC | VCC | 4 |
| AVCC | AVCC | 18 |
| GND | GND | 3, 21 |

### Development sequence

1. Confirm target supply voltage and electrical levels.
2. Connect the ISP signals through a safe interface.
3. Enter programming mode.
4. Read device signature.
5. Validate the expected ATmega328P signature `1E 95 0F`.
6. Implement Flash read.
7. Save the image as BIN on microSD.
8. Calculate and record image integrity information.
9. Verify the saved image against a second read.
10. Only after read/verify is stable, implement erase/program.

## Status table

| MCU | Interface | Detect | Read | Write | Verify | Status |
|---|---|---:|---:|---:|---:|---|
| ATmega328P | AVR ISP | Planned | Planned | Planned | Planned | First target |

## Rule

No write/erase operation is implemented until the read and verification path has been bench-tested successfully.
