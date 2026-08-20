#include <Arduino.h>
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "board_config.h"
#include "atmega328p_isp.h"
#include "storage_test.h"
#include "storage_manager.h"
#include "settings.h"
#include "network_manager.h"
#include "ha_integration.h"
#include "ui_manager.h"

// PanLee ZX3D50CE08S-V16-USRC / WT32-S3-WROVER
// Display configuration is the proven TUL ST7796/FT5x06 setup.
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

static void drawProSpeaker(int x, int y, bool left) {
    const uint16_t white = TFT_WHITE;
    const uint16_t gray = 0x5AEB;
    const uint16_t cyan = TFT_CYAN;

    if (left) {
        tft.fillRect(x, y + 15, 10, 30, gray);
        tft.fillRect(x + 10, y + 5, 4, 50, white);
        tft.fillTriangle(x + 14, y + 5, x + 40, y - 5, x + 40, y + 65, white);
        tft.fillRoundRect(x + 40, y - 5, 8, 70, 4, white);
        tft.fillCircle(x + 40, y + 30, 8, cyan);
    } else {
        tft.fillRect(x + 50, y + 15, 10, 30, gray);
        tft.fillRect(x + 46, y + 5, 4, 50, white);
        tft.fillTriangle(x + 46, y + 5, x + 20, y - 5, x + 20, y + 65, white);
        tft.fillRoundRect(x + 12, y - 5, 8, 70, 4, white);
        tft.fillCircle(x + 20, y + 30, 8, cyan);
    }
}

static void showBootAnimation() {
    tft.fillScreen(TFT_BLACK);
    for (int i = 0; i <= 150; i += 6) {
        tft.fillScreen(TFT_BLACK);
        drawProSpeaker(180 - i, 120, true);
        drawProSpeaker(240 + i, 120, false);
        delay(10);
    }
    tft.setTextColor(TFT_YELLOW);
    tft.setTextSize(4);
    tft.setCursor(200, 110);
    tft.print("TUL");
    tft.setTextColor(TFT_CYAN);
    tft.setTextSize(2);
    tft.setCursor(125, 165);
    tft.print("Digital Audio System");
    delay(1200);
}

static void showTargetResult(const uint8_t signature[3], bool ispOk) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(3);
    tft.setTextColor(TFT_CYAN);
    tft.setCursor(35, 35);
    tft.print("TUL MCU");
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(35, 80);
    tft.print("FIRMWARE READER");
    tft.drawRect(25, 120, 430, 70, TFT_CYAN);
    tft.setCursor(45, 145);
    tft.setTextColor(ispOk ? TFT_GREEN : TFT_RED);
    tft.print(ispOk ? "ATmega328P OK" : "ISP FAILED");
    if (ispOk) {
        tft.setTextColor(TFT_WHITE);
        tft.setCursor(45, 215);
        tft.printf("SIGNATURE %02X %02X %02X", signature[0], signature[1], signature[2]);
    }
}

void setup() {
    Serial.begin(115200);
    delay(300);

    settingsBegin();
    pinMode(45, OUTPUT);
    analogWrite(45, settings().brightness);

    tft.init();
    tft.setRotation(1);
    tft.invertDisplay(true);
    showBootAnimation();

    const bool sdTestOk = runStorageAndMemoryTest();
    storageBegin();
    Serial.printf("Storage test: %s\n", sdTestOk ? "PASS" : "FAIL");

    networkBegin();
    delay(100);
    networkLoop();
    haBegin();

    Serial.println();
    Serial.println("TUL MCU Firmware Reader");
    Serial.println("ATmega328P ISP Proof of Concept");
    Serial.println("Utility / Engineering / Network framework enabled");
    Serial.println("No erase/write operations");

    isp.begin();
    uint8_t signature[3] = {0};
    const bool ispOk = isp.readSignature(signature);
    if (ispOk) {
        Serial.printf("ATmega328P signature: %02X %02X %02X\n", signature[0], signature[1], signature[2]);
        Serial.println((signature[0] == 0x1E && signature[1] == 0x95 && signature[2] == 0x0F) ? "TARGET OK" : "UNEXPECTED SIGNATURE");
    } else {
        Serial.println("ISP communication failed");
    }

    showTargetResult(signature, ispOk);
    delay(1000);
    tului::mainMenu(tft);
}

void loop() {
    networkLoop();
    haLoop();
    delay(50);
}
