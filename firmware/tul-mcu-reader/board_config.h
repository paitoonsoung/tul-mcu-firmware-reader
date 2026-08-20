#pragma once

// J5 on the actual PanLee ZX3D50CE08S-V16-USRC board.
// Target interface uses GPIO only; J5 +5V is intentionally unused.
constexpr uint8_t PIN_ISP_MOSI = 10;
constexpr uint8_t PIN_ISP_MISO = 11;
constexpr uint8_t PIN_ISP_SCK = 12;
constexpr uint8_t PIN_TARGET_RESET = 13;

constexpr uint32_t ISP_HALF_PERIOD_US = 100;

// WT32-SC01 Plus onboard microSD (SPI).
constexpr uint8_t PIN_SD_CS = 41;
constexpr uint8_t PIN_SD_MOSI = 40;
constexpr uint8_t PIN_SD_SCK = 39;
constexpr uint8_t PIN_SD_MISO = 38;
