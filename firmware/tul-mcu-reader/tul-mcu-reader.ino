#include <Arduino.h>
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <SD.h>
#include "board_config.h"
#include "atmega328p_isp.h"
#include "storage_test.h"

// PanLee ZX3D50CE08S-V16-USRC / WT32-S3-WROVER
// Display configuration adapted from the proven TUL display code.
class LGFX_TUL : public lgfx::LGFX_Device {
    lgfx::Panel_ST7796 _panel_instance;
    lgfx::Bus_Parallel8 _bus_instance;
    lgfx::Touch_FT5x06 _touch_instance;

public:
    LGFX_TUL() {
        auto cfg = _bus_instance.config();
        cfg.freq_write = 20000000;
        cfg.pin_wr = 47;
        cfg.pin_rd = -1;
        cfg.pin_rs = 0;
        cfg.pin_d0 = 9;
        cfg.pin_d1 = 46;
        cfg.pin_d2 = 3;
        cfg.pin_d3 = 8;
        cfg.pin_d4 = 18;
        cfg.pin_d5 = 17;
        cfg.pin_d6 = 16;
        cfg.pin_d7 = 15;
        _bus_instance.config(cfg);
        _panel_instance.setBus(&_bus_instance);

        auto p_cfg = _panel_instance.config();
        p_cfg.pin_rst = 4;
        p_cfg.panel_width = 320;
        p_cfg.panel_height = 480;
        _panel_instance.config(p_cfg);

        auto t_cfg = _touch_instance.config();
        t_cfg.pin_sda = 6;
        t_cfg.pin_scl = 5;
        t_cfg.pin_int = 7;
        t_cfg.i2c_port = 1;
        t_cfg.x_min = 0;
        t_cfg.x_max = 319;
        t_cfg.y_min = 0;
        t_cfg.y_max = 479;
        _touch_instance.config(t_cfg);
        _panel_instance.setTouch(&_touch_instance);

        setPanel(&_panel_instance);
    }
};

LGFX_TUL tft;
Atmega328pIsp isp;

void drawProSpeaker(int x, int y, bool isLeft) {
    const uint16_t white = TFT_WHITE;
    const uint16_t gray = 0x5AEB;
    const uint16_t cyan = TFT_CYAN;

    if (isLeft) {
        tft.fillRect(x, y + 15, 10, 30, gray);
        tft.fillRect(x + 10, y + 5, 4, 50, white);
        tft.fillTriangle(x + 14, y + 5, x + 40, y - 5,
                         x + 40, y + 65, white);
        tft.fillTriangle(x + 14, y + 55, x + 40, y - 5,
                         x + 40, y + 65, white);
        tft.fillRoundRect(x + 40, y - 5, 8, 70, 4, white);
        tft.fillCircle(x + 40, y + 30, 8, cyan);
    } else {
        tft.fillRect(x + 50, y + 15, 10, 30, gray);
        tft.fillRect(x + 46, y + 5, 4, 50, white);
        tft.fillTriangle(x + 46, y + 5, x + 20, y - 5,
                         x + 20, y + 65, white);
        tft.fillTriangle(x + 46, y + 55, x + 20, y - 5,
                         x + 20, y + 65, white);
        tft.fillRoundRect(x + 12, y - 5, 8, 70, 4, white);
        tft.fillCircle(x + 20, y + 30, 8, cyan);
    }
}

void showBootAnimation() {
    tft.fillScreen(TFT_BLACK);

    const int centerY = 120;
    const int maxDist = 150;

    for (int i = 0; i <= maxDist; i += 6) {
        tft.fillScreen(TFT_BLACK);
        drawProSpeaker(180 - i, centerY, true);
        drawProSpeaker(240 + i, centerY, false);
        delay(10);
    }

    tft.setTextColor(TFT_YELLOW);
    tft.setTextSize(4);
    const char *title = "TUL";
    for (uint8_t i = 0; title[i] != '\0'; ++i) {
        tft.setCursor(200 + (i * 25), centerY - 10);
        tft.print(title[i]);
        delay(150);
    }

    tft.setTextColor(TFT_CYAN);
    tft.setTextSize(2);
    const char *subtitle = "Digital Audio System";
    for (uint8_t i = 0; subtitle[i] != '\0'; ++i) {
        tft.setCursor(125 + (i * 12), centerY + 45);
        tft.print(subtitle[i]);
        delay(50);
    }

    delay(1500);
}

void showSystemInfo(bool sdOk) {
    const uint32_t ramFree = ESP.getFreeHeap();
    const uint32_t psramTotal = ESP.getPsramSize();
    const uint32_t psramFree = ESP.getFreePsram();
    const uint32_t flashSize = ESP.getFlashChipSize();

    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_CYAN);
    tft.setTextSize(2);
    tft.setCursor(20, 18);
    tft.print("TUL SYSTEM DIAGNOSTIC");

    tft.setTextColor(TFT_WHITE);
    tft.setCursor(20, 55);
    tft.printf("RAM FREE   %lu KB", ramFree / 1024UL);

    tft.setCursor(20, 82);
    tft.printf("PSRAM      %lu / %lu KB",
               psramFree / 1024UL, psramTotal / 1024UL);

    tft.setCursor(20, 109);
    tft.printf("FLASH      %lu MB", flashSize / (1024UL * 1024UL));

    tft.setCursor(20, 136);
    tft.print("microSD    ");
    tft.setTextColor(sdOk ? TFT_GREEN : TFT_RED);
    tft.print(sdOk ? "READY" : "NOT READY");

    if (sdOk) {
        tft.setTextColor(TFT_WHITE);
        tft.setCursor(20, 163);
        tft.printf("CARD       %llu MB",
                   SD.cardSize() / (1024ULL * 1024ULL));

        tft.setCursor(20, 190);
        tft.printf("FILESYSTEM %llu MB",
                   SD.totalBytes() / (1024ULL * 1024ULL));

        tft.setCursor(20, 217);
        tft.printf("USED       %llu MB",
                   SD.usedBytes() / (1024ULL * 1024ULL));
    }

    tft.setTextColor(TFT_CYAN);
    tft.setCursor(20, 270);
    tft.print("MEMORY / SD TEST COMPLETE");
    delay(2500);
}

void showReaderReady() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_CYAN);
    tft.setTextSize(3);
    tft.setCursor(35, 45);
    tft.print("TUL MCU");

    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setCursor(35, 95);
    tft.print("FIRMWARE READER");

    tft.drawRect(25, 130, 270, 65, TFT_CYAN);
    tft.setTextSize(2);
    tft.setCursor(45, 155);
    tft.print("ISP READY");
}

void setup() {
    Serial.begin(115200);
    delay(300);

    pinMode(45, OUTPUT);
    analogWrite(45, 200);

    tft.init();
    tft.setRotation(1);
    tft.invertDisplay(true);

    showBootAnimation();

    const bool sdOk = runStorageAndMemoryTest();
    showSystemInfo(sdOk);
    showReaderReady();

    Serial.println();
    Serial.println("TUL MCU Firmware Reader");
    Serial.println("ATmega328P ISP Proof of Concept");
    Serial.println("Display: ST7796 / Parallel 8-bit");
    Serial.println("Storage: microSD / SPI");
    Serial.println("No erase/write operations");

    isp.begin();

    uint8_t signature[3] = {0};
    if (!isp.readSignature(signature)) {
        Serial.println("ISP communication failed");
        tft.setTextColor(TFT_RED);
        tft.setCursor(45, 220);
        tft.print("ISP FAILED");
        return;
    }

    Serial.printf("ATmega328P signature: %02X %02X %02X\n",
                  signature[0], signature[1], signature[2]);

    tft.setCursor(45, 220);
    if (signature[0] == 0x1E &&
        signature[1] == 0x95 &&
        signature[2] == 0x0F) {
        Serial.println("TARGET OK");
        tft.setTextColor(TFT_GREEN);
        tft.print("ATmega328P OK");
    } else {
        Serial.println("UNEXPECTED SIGNATURE");
        tft.setTextColor(TFT_RED);
        tft.print("SIGNATURE ERROR");
    }
}

void loop() {
    delay(1000);
}
