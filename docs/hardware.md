# TUL MCU Firmware Reader Hardware

## Actual prototype board

- Main board: PanLee ZX3D50CE08S-V16-USRC
- Board marking: 240221
- Wireless module: WT32-S3-WROVER (Wireless-Tag)
- MCU platform: ESP32-S3
- User interface: onboard display/touch hardware
- Storage: onboard microSD connector

The physical prototype is already assembled in its enclosure. The repository uses the actual prototype as the hardware reference; connector assignments below are taken from the PCB pinout provided for this board.

## Onboard connectors

### J5 — I/O

Pin order shown on the PCB pinout:

| Pin | Signal |
|---:|---|
| 1 | +5V |
| 2 | GND |
| 3 | GPIO10 |
| 4 | GPIO11 |
| 5 | GPIO12 |
| 6 | GPIO13 |
| 7 | GPIO14 |
| 8 | GPIO21 |

This connector is the primary candidate for exposing GPIO to the external target interface. It must not be connected to a target MCU until the target voltage and final signal direction are defined.

### J3 — DEBUG

Pin order shown on the PCB pinout:

### J3 — DEBUG

Pin order shown on the PCB pinout:

| Pin | Signal           |
| --: | ---------------- |
|   1 | 5V               |
|   2 | 3V3              |
|   3 | UART_DEV(0) TxD |
|   4 | UART_DEV(0) RxD |
|   5 | RESET (EN)       |
|   6 | BOOT (GPIO0)     |
|   7 | GND              |

J3 is treated as the board debug/serial connector, not as the target-MCU programming connector.

J3 is treated as the board debug/serial connector, not as the target-MCU programming connector.

J3 is treated as the board debug/serial connector, not as the target-MCU programming connector.

### J4 — SPK

Speaker connector. It is outside the initial programmer signal path.

### USB-C

Used for board power/programming/USB functions as supported by the board design. Exact USB role will be verified during firmware bring-up.

### microSD

The WT32-SC01 Plus documentation identifies the onboard microSD interface as SPI and assigns:

| Signal | ESP32-S3 GPIO |
|---|---:|
| SD_CS | GPIO41 |
| SD_MOSI / SD_DI | GPIO40 |
| SD_SCK / SD_CLK | GPIO39 |
| SD_MISO / SD_DO | GPIO38 |

These pins are now recorded in `firmware/tul-mcu-reader/board_config.h` and are used by the storage diagnostic. citeturn0search30turn0search33

The intended firmware data flow is:

```text
Target MCU
    |
    | Read
    v
ESP32-S3 RAM / PSRAM
    |
    | Stream chunks
    v
microSD
    |
    v
firmware.bin
```

## Hardware boundary

The programmer hardware must provide a controlled interface between the ESP32-S3 and the target MCU.

Candidate target signals include:

- Target VCC
- Target GND
- Target RESET
- Target TX/RX
- Target clock/data signals where required by the selected programming protocol

The J5 GPIO signals are board GPIO resources, not yet a complete target-MCU protocol assignment.

## Electrical requirements

- Confirm target voltage before driving any target signal.
- Do not assume every target MCU is 3.3 V tolerant.
- Add level shifting or voltage control where required.
- Define a safe target-power policy before implementing programming operations.
- Never connect an unknown target signal directly to a GPIO until its voltage and direction are known.

## Bring-up order

1. Verify WT32-S3-WROVER power and ESP32-S3 boot.
2. Verify display.
3. Verify touch input.
4. Verify microSD.
5. Verify J3 debug/serial output.
6. Verify J5 GPIO levels and safe GPIO operation.
7. Select the first real target MCU.
8. Define and validate the target interface electrically.
9. Implement the first transport and MCU driver only after the electrical interface is confirmed.

## Current status

The board-level connector pinout is documented:

- J5: +5V, GND, GPIO10, GPIO11, GPIO12, GPIO13, GPIO14, GPIO21
- J3: +5V, +3.3V, TXD, RXD, EN (RST), GND, GND
- microSD: CS=GPIO41, MOSI=GPIO40, SCK=GPIO39, MISO=GPIO38

The target-MCU protocol and target connector wiring are still intentionally undefined. No target MCU pin assignment is being assumed at this stage.
