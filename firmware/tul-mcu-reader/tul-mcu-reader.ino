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

class LGFX_TUL : public lgfx::LGFX_Device {
    lgfx::Panel_ST7796 _panel_instance;
    lgfx::Bus_Parallel8 _bus_instance;
    lgfx::Touch_FT5x06 _touch_instance;
public:
    LGFX_TUL() {
        auto bus = _bus_instance.config();
        bus.freq_write = 20000000;
        bus.pin_wr = 47; bus.pin_rd = -1; bus.pin_rs = 0;
        bus.pin_d0 = 9; bus.pin_d1 = 46; bus.pin_d2 = 3; bus.pin_d3 = 8;
        bus.pin_d4 = 18; bus.pin_d5 = 17; bus.pin_d6 = 16; bus.pin_d7 = 15;
        _bus_instance.config(bus);
        _panel_instance.setBus(&_bus_instance);

        auto panel = _panel_instance.config();
        panel.pin_cs = -1; panel.pin_rst = 4; panel.pin_busy = -1;
        panel.memory_width = 320; panel.memory_height = 480;
        panel.panel_width = 320; panel.panel_height = 480;
        panel.offset_x = 0; panel.offset_y = 0;
        panel.offset_rotation = 2;
        panel.readable = false; panel.invert = true;
        panel.rgb_order = false; panel.dlen_16bit = false;
        panel.bus_shared = true;
        _panel_instance.config(panel);

        auto touch = _touch_instance.config();
        touch.x_min = 0; touch.x_max = 319;
        touch.y_min = 0; touch.y_max = 479;
        touch.pin_int = 7;
        touch.pin_rst = -1;             // GPIO4 is reserved for the LCD reset line.
        touch.bus_shared = false;
        touch.offset_rotation = 0;
        touch.i2c_port = 0;              // WT32-SC01 PLUS uses I2C0
        touch.i2c_addr = 0x38;
        touch.pin_sda = 6;
        touch.pin_scl = 5;
        touch.freq = 400000;
        _touch_instance.config(touch);
        _panel_instance.setTouch(&_touch_instance);
        setPanel(&_panel_instance);
    }
};

LGFX_TUL tft;

static void drawProSpeaker(int x, int y, bool left) {
    const uint16_t w = TFT_WHITE, g = 0x5AEB, c = TFT_CYAN;
    if (left) {
        tft.fillRect(x, y + 15, 10, 30, g); tft.fillRect(x + 10, y + 5, 4, 50, w);
        tft.fillTriangle(x + 14, y + 5, x + 40, y - 5, x + 40, y + 65, w);
        tft.fillRoundRect(x + 40, y - 5, 8, 70, 4, w); tft.fillCircle(x + 40, y + 30, 8, c);
    } else {
        tft.fillRect(x + 50, y + 15, 10, 30, g); tft.fillRect(x + 46, y + 5, 4, 50, w);
        tft.fillTriangle(x + 46, y + 5, x + 20, y - 5, x + 20, y + 65, w);
        tft.fillRoundRect(x + 12, y - 5, 8, 70, 4, w); tft.fillCircle(x + 20, y + 30, 8, c);
    }
}

static void showBootAnimation() {
    tft.fillScreen(TFT_BLACK);
    for (int i = 0; i <= 150; i += 6) {
        tft.fillScreen(TFT_BLACK);
        drawProSpeaker(180 - i, 120, true); drawProSpeaker(240 + i, 120, false);
        delay(10);
    }
    tft.setTextColor(TFT_YELLOW); tft.setTextSize(4); tft.setCursor(200, 110); tft.print("TUL");
    tft.setTextColor(TFT_CYAN); tft.setTextSize(2); tft.setCursor(125, 165); tft.print("Digital Audio System");
    delay(1200);
}

static void printMemoryDiagnostic() {
    Serial.printf("Internal RAM free: %lu KB\n", ESP.getFreeHeap() / 1024UL);
    Serial.printf("PSRAM free/size: %lu / %lu KB\n", ESP.getFreePsram() / 1024UL, ESP.getPsramSize() / 1024UL);
}

void setup() {
    Serial.begin(115200); delay(300);
    settingsBegin();
    pinMode(45, OUTPUT); analogWrite(45, settings().brightness);
    tft.init();
    tft.setRotation(1);
    tft.invertDisplay(true);
    Serial.println("WT32-SC01 PLUS: FT5x06 SDA=6 SCL=5 INT=7 I2C0 ADDR=0x38");

    showBootAnimation();
    const bool sdTestOk = runStorageAndMemoryTest();
    storageBegin();
    Serial.printf("Storage test: %s\n", sdTestOk ? "PASS" : "FAIL");
    networkBegin();
    haBegin();
    printMemoryDiagnostic();
    tului::mainMenu(tft);
}

void loop() {
    networkLoop();
    haLoop();
    tului::uiLoop(tft);
    delay(5);
}
