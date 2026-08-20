# Utility, Storage and Network Architecture

## Implemented UI

The current firmware adds a touch-driven Utility layer on the WT32-SC01 Plus:

- System information: RAM, PSRAM, Flash and microSD statistics.
- Display settings: brightness value and screensaver timeout stored in ESP32 NVS.
- Storage: capacity/free-space display and local-only microSD format confirmation.
- Network: WiFi enable/disable, asynchronous scan, saved SSID/password storage and BLE enable/disable.
- On-screen keyboard: letters, numbers, SHIFT, SPACE, backspace and ENTER.
- Engineering mode: RAM/PSRAM/SD/WiFi/BLE status plus safe touch/SD checks.
- About TUL page.

## microSD format policy

Formatting is deliberately a local touch-screen operation. Home Assistant is not given a remote format command because a remote automation must never be able to erase the firmware archive accidentally.

## Home Assistant

`ha_integration.cpp` provides optional MQTT telemetry and Home Assistant MQTT Discovery. It uses `PubSubClient` only when that library is available at compile time. If the library is absent, the firmware remains usable and reports the HA connection as unavailable.

Set the broker parameters in `firmware/tul-mcu-reader/ha_config.h` after confirming the broker used by the existing TUL Home Assistant system.

Published state topic:

```text
tul/mcu_reader/state
```

Discovery topic:

```text
homeassistant/sensor/tul_mcu_reader/config
```

The first telemetry payload contains online status, WiFi SSID/IP/RSSI, SD free space and free RAM.

## Current boundary

Target MCU erase/write is still intentionally not enabled. The ATmega328P ISP proof-of-concept remains a read/signature stage until the write path is bench-tested independently.
