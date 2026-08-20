#pragma once
#include <Arduino.h>
#include <LovyanGFX.hpp>
#include "network_manager.h"
#include "settings.h"
#include "storage_manager.h"
#include "atmega328p_isp.h"
#include "board_config.h"

namespace tului {

enum class UiState : uint8_t {
    MAIN, MCU_READER, UTILITY, SYSTEM_INFO, DISPLAY_SCREEN, NETWORK,
    STORAGE, ENGINEERING, ABOUT, WIFI_SCAN, KEYBOARD, FORMAT_CONFIRM,
    TOUCH_TEST, SD_TEST, GPIO_TEST
};
enum class KeyboardMode : uint8_t { SSID, PASSWORD };

static UiState state = UiState::MAIN;
static UiState previousState = UiState::MAIN;
static KeyboardMode keyboardMode = KeyboardMode::SSID;
static bool uiDirty = true;
static bool touchWasDown = false;
static String kbValue;
static String selectedSsid;
static bool kbShift = true;
static bool kbPassword = false;
static String testMessage;
static uint8_t signature[3] = {0, 0, 0};
static bool signatureOk = false;
static bool flashOk = false;

static bool inRect(uint16_t x, uint16_t y, int rx, int ry, int rw, int rh) {
    return x >= rx && x < rx + rw && y >= ry && y < ry + rh;
}

static void gotoState(UiState next) {
    if (state == next) return;
    previousState = state;
    state = next;
    uiDirty = true;
}

template <typename D>
static void header(D &d, const char *title) {
    d.fillScreen(TFT_BLACK);
    d.setTextSize(2);
    d.setTextColor(TFT_CYAN);
    d.setCursor(18, 12);
    d.print(title);
    d.drawFastHLine(18, 36, 444, 0x03EF);
}

template <typename D>
static void button(D &d, int x, int y, int w, int h, const char *label,
                   uint16_t fill = 0x2945, uint16_t border = TFT_WHITE) {
    d.fillRoundRect(x, y, w, h, 7, fill);
    d.drawRoundRect(x, y, w, h, 7, border);
    d.setTextColor(TFT_WHITE);
    d.setTextSize(2);
    d.setCursor(x + 10, y + (h - 16) / 2);
    d.print(label);
}

template <typename D>
static bool touchPressed(D &d, uint16_t &x, uint16_t &y) {
    uint16_t px = 0, py = 0;
    const bool down = d.getTouch(&px, &py);
    const bool edge = down && !touchWasDown;
    touchWasDown = down;
    if (!edge) return false;
    x = px;
    y = py;
    return true;
}

template <typename D> static void drawMain(D &d) {
    header(d, "TUL MCU FIRMWARE READER");
    button(d, 35, 70, 190, 70, "MCU READER", 0x801F);
    button(d, 255, 70, 190, 70, "UTILITY", 0x2D7F);
    button(d, 35, 165, 190, 70, "ENGINEERING", 0xFD20);
    button(d, 255, 165, 190, 70, "SYSTEM INFO", 0x05FF);
    d.setTextColor(TFT_GREEN); d.setCursor(35, 250); d.print("SD READY");
    d.setTextColor(TFT_CYAN); d.setCursor(255, 250); d.print("TUL SYSTEM");
}

template <typename D> static void drawMcuReader(D &d) {
    header(d, "MCU READER");
    d.setTextColor(TFT_WHITE); d.setTextSize(2);
    d.setCursor(18, 52); d.print("Target: ATmega328P");
    d.setCursor(18, 78);
    d.printf("Signature: %02X %02X %02X", signature[0], signature[1], signature[2]);
    d.setTextColor(signatureOk ? TFT_GREEN : TFT_YELLOW);
    d.setCursor(18, 104); d.print(signatureOk ? "TARGET DETECTED" : "NOT DETECTED");
    button(d, 18, 135, 205, 55, "READ SIGNATURE", 0x801F);
    button(d, 235, 135, 205, 55, "READ FLASH -> SD", 0x2D7F);
    d.setTextColor(flashOk ? TFT_GREEN : TFT_WHITE);
    d.setCursor(18, 208);
    d.print(flashOk ? "/ATmega328P_flash.bin READY" : "Output: /ATmega328P_flash.bin");
    button(d, 330, 260, 120, 45, "BACK", 0xF800);
}

template <typename D> static void drawUtility(D &d) {
    header(d, "TUL UTILITY");
    button(d, 18, 50, 135, 42, "SYSTEM INFO", 0x05FF);
    button(d, 165, 50, 135, 42, "DISPLAY", 0x801F);
    button(d, 312, 50, 135, 42, "NETWORK", 0x2D7F);
    button(d, 18, 105, 135, 42, "STORAGE", 0x05E0);
    button(d, 165, 105, 135, 42, "ENGINEER", 0xFD20);
    button(d, 312, 105, 135, 42, "ABOUT TUL", 0x05FF);
    button(d, 330, 260, 120, 45, "BACK", 0xF800);
}

template <typename D> static void drawSystemInfo(D &d) {
    header(d, "SYSTEM INFO");
    d.setTextColor(TFT_WHITE); d.setTextSize(2);
    d.setCursor(18, 52); d.print("CPU       ESP32-S3");
    d.setCursor(18, 76); d.printf("RAM FREE  %lu KB", ESP.getFreeHeap() / 1024UL);
    d.setCursor(18, 100); d.printf("PSRAM     %lu / %lu KB", ESP.getFreePsram() / 1024UL, ESP.getPsramSize() / 1024UL);
    d.setCursor(18, 124); d.printf("FLASH     %lu MB", ESP.getFlashChipSize() / (1024UL * 1024UL));
    d.setCursor(18, 148); d.printf("SD CARD   %llu MB", storageCapacityBytes() / (1024ULL * 1024ULL));
    d.setCursor(18, 172); d.printf("SD FREE   %llu MB", storageFreeBytes() / (1024ULL * 1024ULL));
    d.setCursor(18, 196); d.printf("WiFi      %.25s", networkSSID());
    d.setCursor(18, 220); d.printf("IP        %s", networkIP());
    d.setCursor(18, 244); d.printf("RSSI      %d dBm", networkRSSI());
    button(d, 330, 260, 120, 45, "BACK", 0xF800);
}

template <typename D> static void drawDisplayScreen(D &d) {
    header(d, "DISPLAY");
    d.setTextColor(TFT_WHITE); d.setTextSize(2);
    d.setCursor(18, 55); d.printf("Brightness: %u", settings().brightness);
    button(d, 20, 82, 100, 42, "-", 0xF800);
    button(d, 135, 82, 100, 42, "+", 0x05E0);
    button(d, 250, 82, 100, 42, "100%", 0x05FF);
    d.setCursor(18, 145); d.printf("Screensaver: %u sec", settings().screensaverSeconds);
    button(d, 20, 172, 100, 42, "1 MIN", 0x2D7F);
    button(d, 135, 172, 100, 42, "5 MIN", 0x801F);
    button(d, 250, 172, 100, 42, "OFF", 0xF800);
    button(d, 330, 260, 120, 45, "BACK", 0xF800);
}

template <typename D> static void drawStorage(D &d) {
    header(d, "STORAGE");
    d.setTextColor(TFT_WHITE); d.setTextSize(2);
    d.setCursor(18, 55); d.printf("Capacity: %llu MB", storageCapacityBytes() / (1024ULL * 1024ULL));
    d.setCursor(18, 79); d.printf("Free:     %llu MB", storageFreeBytes() / (1024ULL * 1024ULL));
    button(d, 18, 112, 200, 48, "FORMAT SD", 0xF800);
    button(d, 235, 112, 200, 48, "REFRESH", 0x05E0);
    button(d, 330, 260, 120, 45, "BACK", 0xF800);
}

template <typename D> static void drawNetwork(D &d) {
    header(d, "NETWORK");
    d.setTextColor(TFT_WHITE); d.setTextSize(2);
    d.setCursor(18, 52); d.printf("WiFi: %s", settings().wifiEnabled ? "ON" : "OFF");
    d.setCursor(18, 76); d.printf("SSID: %.25s", networkSSID());
    d.setCursor(18, 100); d.printf("IP: %s", networkIP());
    button(d, 18, 125, 130, 44, "WIFI ON/OFF", 0x2D7F);
    button(d, 160, 125, 130, 44, "SCAN", 0x05E0);
    button(d, 302, 125, 130, 44, "BLE ON/OFF", 0x801F);
    button(d, 18, 185, 130, 44, "KEYBOARD", 0xFD20);
    button(d, 160, 185, 130, 44, "SAVED", 0x05FF);
    button(d, 330, 260, 120, 45, "BACK", 0xF800);
}

template <typename D> static void drawEngineering(D &d) {
    header(d, "ENGINEERING MODE");
    d.setTextColor(TFT_WHITE); d.setTextSize(2);
    d.setCursor(18, 55); d.printf("RAM   %lu KB free", ESP.getFreeHeap() / 1024UL);
    d.setCursor(18, 79); d.printf("PSRAM %lu KB free", ESP.getFreePsram() / 1024UL);
    d.setCursor(18, 103); d.printf("SD    %s", storageCapacityBytes() ? "READY" : "FAIL");
    d.setCursor(18, 127); d.printf("WiFi  %s", settings().wifiEnabled ? "ENABLED" : "OFF");
    d.setCursor(18, 151); d.printf("BLE   %s", settings().bleEnabled ? "ENABLED" : "OFF");
    button(d, 18, 185, 130, 44, "TOUCH TEST", 0x2D7F);
    button(d, 160, 185, 130, 44, "SD TEST", 0x05E0);
    button(d, 302, 185, 130, 44, "GPIO TEST", 0xFD20);
    button(d, 330, 260, 120, 45, "BACK", 0xF800);
}

template <typename D> static void drawTest(D &d, const char *title) {
    header(d, title);
    d.setTextColor(TFT_WHITE); d.setTextSize(2);
    d.setCursor(18, 60); d.print(testMessage.length() ? testMessage : "READY");
    if (state == UiState::TOUCH_TEST) {
        d.drawRect(18, 100, 420, 110, TFT_CYAN);
        d.setCursor(30, 120); d.print("Touch inside this box");
    } else if (state == UiState::GPIO_TEST) {
        d.setCursor(18, 105); d.printf("MOSI GPIO %u", PIN_ISP_MOSI);
        d.setCursor(18, 130); d.printf("MISO GPIO %u", PIN_ISP_MISO);
        d.setCursor(18, 155); d.printf("SCK  GPIO %u", PIN_ISP_SCK);
        d.setCursor(18, 180); d.printf("RESET GPIO %u", PIN_TARGET_RESET);
    }
    button(d, 330, 260, 120, 45, "BACK", 0xF800);
}

template <typename D> static void drawScan(D &d) {
    header(d, "WIFI SCAN");
    const int count = networkScanCount();
    d.setTextColor(count == WIFI_SCAN_RUNNING ? TFT_YELLOW : TFT_WHITE);
    d.setCursor(18, 52);
    if (count == WIFI_SCAN_RUNNING) d.print("SCANNING...");
    else d.printf("Found: %d", count < 0 ? 0 : count);
    if (count > 0) {
        const int shown = count > 7 ? 7 : count;
        for (int i = 0; i < shown; ++i) {
            d.setCursor(18, 78 + i * 25);
            d.printf("%d %-21s %d", i + 1, networkScanName(i).c_str(), networkScanRSSI(i));
        }
    }
    button(d, 330, 260, 120, 45, "BACK", 0xF800);
}

template <typename D> static void drawAbout(D &d) {
    header(d, "ABOUT TUL");
    d.setTextColor(TFT_WHITE); d.setTextSize(2);
    d.setCursor(18, 65); d.print("TUL MCU Firmware Reader");
    d.setCursor(18, 95); d.print("Standalone Service Instrument");
    d.setCursor(18, 125); d.print("ESP32-S3 platform");
    button(d, 330, 260, 120, 45, "BACK", 0xF800);
}

template <typename D> static void drawFormatConfirm(D &d) {
    header(d, "FORMAT SD");
    d.setTextColor(TFT_RED); d.setCursor(25, 65); d.print("ALL DATA WILL BE ERASED");
    d.setTextColor(TFT_WHITE); d.setCursor(25, 95); d.print("LOCAL ONLY - CONFIRM?");
    button(d, 25, 140, 190, 55, "CANCEL", 0xFD20);
    button(d, 245, 140, 190, 55, "FORMAT", 0xF800);
}

template <typename D> static void drawKeyboard(D &d) {
    header(d, kbPassword ? "WIFI PASSWORD" : "WIFI SSID");
    d.setTextColor(TFT_WHITE); d.setCursor(12, 42);
    if (kbPassword) for (size_t i = 0; i < kbValue.length(); ++i) d.print('*');
    else d.print(kbValue);
    const char *lower = "qwertyuiopasdfghjklzxcvbnm";
    const char *upper = "QWERTYUIOPASDFGHJKLZXCVBNM";
    for (int i = 0; i < 26; ++i) {
        const int row = i / 10, col = i % 10;
        const int x = 8 + col * 46, y = 70 + row * 38;
        d.fillRoundRect(x, y, 40, 32, 5, (i & 1) ? 0x2D7F : 0x801F);
        d.setCursor(x + 13, y + 8); d.print(kbShift ? upper[i] : lower[i]);
    }
    for (int i = 0; i < 10; ++i) {
        const int x = 8 + i * 46;
        d.fillRoundRect(x, 188, 40, 32, 5, 0x2945);
        d.setCursor(x + 13, 196); d.print(i == 9 ? '0' : char('1' + i));
    }
    button(d, 8, 230, 90, 40, "SHIFT", 0xFFE0);
    button(d, 108, 230, 170, 40, "SPACE", 0x2945);
    button(d, 288, 230, 90, 40, "BKSP", 0xF800);
    button(d, 388, 230, 80, 40, "ENTER", 0x05E0);
}

template <typename D> static void drawCurrent(D &d) {
    switch (state) {
        case UiState::MAIN: drawMain(d); break;
        case UiState::MCU_READER: drawMcuReader(d); break;
        case UiState::UTILITY: drawUtility(d); break;
        case UiState::SYSTEM_INFO: drawSystemInfo(d); break;
        case UiState::DISPLAY_SCREEN: drawDisplayScreen(d); break;
        case UiState::NETWORK: drawNetwork(d); break;
        case UiState::STORAGE: drawStorage(d); break;
        case UiState::ENGINEERING: drawEngineering(d); break;
        case UiState::ABOUT: drawAbout(d); break;
        case UiState::WIFI_SCAN: drawScan(d); break;
        case UiState::TOUCH_TEST: drawTest(d, "TOUCH TEST"); break;
        case UiState::SD_TEST: drawTest(d, "SD CARD TEST"); break;
        case UiState::GPIO_TEST: drawTest(d, "GPIO TEST"); break;
        case UiState::KEYBOARD: drawKeyboard(d); break;
        case UiState::FORMAT_CONFIRM: drawFormatConfirm(d); break;
    }
    uiDirty = false;
}

static void startKeyboard(KeyboardMode mode, bool password, const String &initial = String()) {
    keyboardMode = mode; kbPassword = password; kbShift = true; kbValue = initial; kbValue.reserve(64); gotoState(UiState::KEYBOARD);
}

template <typename D> static void handleKeyboard(D &, uint16_t x, uint16_t y) {
    if (y >= 70 && y < 184) {
        const int row = (y - 70) / 38, col = x / 46, index = row * 10 + col;
        if (index < 26 && col < 10) {
            const char *lower = "qwertyuiopasdfghjklzxcvbnm";
            const char *upper = "QWERTYUIOPASDFGHJKLZXCVBNM";
            kbValue += kbShift ? upper[index] : lower[index];
        }
    } else if (y >= 188 && y < 222 && x < 460) {
        const int n = x / 46; kbValue += n == 9 ? '0' : char('1' + n);
    } else if (y >= 230 && y < 275) {
        if (x < 100) kbShift = !kbShift;
        else if (x < 288) kbValue += ' ';
        else if (x < 388) { if (kbValue.length()) kbValue.remove(kbValue.length() - 1); }
        else if (keyboardMode == KeyboardMode::SSID) { selectedSsid = kbValue; startKeyboard(KeyboardMode::PASSWORD, true); return; }
        else { networkSaveCredentials(selectedSsid, kbValue); networkConnectSaved(); gotoState(UiState::NETWORK); return; }
    }
    uiDirty = true;
}

template <typename D> static void handlePressed(D &d, uint16_t x, uint16_t y) {
    switch (state) {
        case UiState::MAIN:
            if (inRect(x,y,35,70,190,70)) gotoState(UiState::MCU_READER);
            else if (inRect(x,y,255,70,190,70)) gotoState(UiState::UTILITY);
            else if (inRect(x,y,35,165,190,70)) gotoState(UiState::ENGINEERING);
            else if (inRect(x,y,255,165,190,70)) gotoState(UiState::SYSTEM_INFO);
            break;
        case UiState::MCU_READER:
            if (inRect(x,y,18,135,205,55)) {
                Atmega328pIsp isp; isp.begin(); signatureOk = isp.readSignature(signature); uiDirty = true;
            } else if (inRect(x,y,235,135,205,55)) {
                Atmega328pIsp isp; isp.begin();
                d.fillScreen(TFT_BLACK); d.setTextColor(TFT_YELLOW); d.setTextSize(2); d.setCursor(20,120); d.print("READING FLASH TO SD...");
                flashOk = isp.readFlashToSd("/ATmega328P_flash.bin");
                uiDirty = true;
            } else if (inRect(x,y,330,260,120,45)) gotoState(UiState::MAIN);
            break;
        case UiState::UTILITY:
            if (inRect(x,y,18,50,135,42)) gotoState(UiState::SYSTEM_INFO);
            else if (inRect(x,y,165,50,135,42)) gotoState(UiState::DISPLAY_SCREEN);
            else if (inRect(x,y,312,50,135,42)) gotoState(UiState::NETWORK);
            else if (inRect(x,y,18,105,135,42)) gotoState(UiState::STORAGE);
            else if (inRect(x,y,165,105,135,42)) gotoState(UiState::ENGINEERING);
            else if (inRect(x,y,312,105,135,42)) gotoState(UiState::ABOUT);
            else if (inRect(x,y,330,260,120,45)) gotoState(UiState::MAIN);
            break;
        case UiState::SYSTEM_INFO: if (inRect(x,y,330,260,120,45)) gotoState(UiState::MAIN); break;
        case UiState::DISPLAY_SCREEN:
            if (inRect(x,y,20,82,100,42)) { settings().brightness = settings().brightness > 20 ? settings().brightness - 20 : 0; analogWrite(45, settings().brightness); settingsSave(); uiDirty=true; }
            else if (inRect(x,y,135,82,100,42)) { settings().brightness = settings().brightness < 235 ? settings().brightness + 20 : 255; analogWrite(45, settings().brightness); settingsSave(); uiDirty=true; }
            else if (inRect(x,y,250,82,100,42)) { settings().brightness=255; analogWrite(45,255); settingsSave(); uiDirty=true; }
            else if (inRect(x,y,20,172,100,42)) { settings().screensaverSeconds=60; settingsSave(); uiDirty=true; }
            else if (inRect(x,y,135,172,100,42)) { settings().screensaverSeconds=300; settingsSave(); uiDirty=true; }
            else if (inRect(x,y,250,172,100,42)) { settings().screensaverSeconds=0; settingsSave(); uiDirty=true; }
            else if (inRect(x,y,330,260,120,45)) gotoState(UiState::UTILITY);
            break;
        case UiState::STORAGE:
            if (inRect(x,y,18,112,200,48)) gotoState(UiState::FORMAT_CONFIRM);
            else if (inRect(x,y,235,112,200,48)) { storageBegin(); uiDirty=true; }
            else if (inRect(x,y,330,260,120,45)) gotoState(UiState::UTILITY);
            break;
        case UiState::FORMAT_CONFIRM:
            if (inRect(x,y,25,140,190,55)) gotoState(UiState::STORAGE);
            else if (inRect(x,y,245,140,190,55)) { d.fillScreen(TFT_BLACK); d.setTextColor(TFT_YELLOW); d.setCursor(25,120); d.print("FORMATTING..."); const bool ok=storageFormat(); d.setTextColor(ok?TFT_GREEN:TFT_RED); d.setCursor(25,160); d.print(ok?"FORMAT COMPLETE":"FORMAT FAILED"); delay(900); gotoState(UiState::STORAGE); }
            break;
        case UiState::NETWORK:
            if (inRect(x,y,18,125,130,44)) { networkToggleWiFi(); uiDirty=true; }
            else if (inRect(x,y,160,125,130,44)) { gotoState(UiState::WIFI_SCAN); networkScanWiFi(); uiDirty=true; }
            else if (inRect(x,y,302,125,130,44)) { networkToggleBLE(); uiDirty=true; }
            else if (inRect(x,y,18,185,130,44)) startKeyboard(KeyboardMode::SSID,false);
            else if (inRect(x,y,160,185,130,44)) { networkConnectSaved(); uiDirty=true; }
            else if (inRect(x,y,330,260,120,45)) gotoState(UiState::UTILITY);
            break;
        case UiState::WIFI_SCAN:
            if (inRect(x,y,330,260,120,45)) gotoState(UiState::NETWORK);
            else { const int count=networkScanCount(); for(int i=0;i<count && i<7;++i) if(inRect(x,y,10,70+i*26,450,25)){ selectedSsid=networkScanName(i); startKeyboard(KeyboardMode::PASSWORD,true); break; } }
            break;
        case UiState::ENGINEERING:
            if (inRect(x,y,18,185,130,44)) { testMessage="Touch test ACTIVE"; gotoState(UiState::TOUCH_TEST); }
            else if (inRect(x,y,160,185,130,44)) { testMessage=storageCapacityBytes()?"SD TEST: CARD READY":"SD TEST: NO CARD"; gotoState(UiState::SD_TEST); }
            else if (inRect(x,y,302,185,130,44)) { testMessage="GPIO TEST: ISP PINS READY"; gotoState(UiState::GPIO_TEST); }
            else if (inRect(x,y,330,260,120,45)) gotoState(UiState::MAIN);
            break;
        case UiState::TOUCH_TEST:
            if (inRect(x,y,18,100,420,110)) { testMessage="Touch OK X="+String(x)+" Y="+String(y); uiDirty=true; }
            else if (inRect(x,y,330,260,120,45)) gotoState(UiState::ENGINEERING);
            break;
        case UiState::SD_TEST: if (inRect(x,y,330,260,120,45)) gotoState(UiState::ENGINEERING); break;
        case UiState::GPIO_TEST: if (inRect(x,y,330,260,120,45)) gotoState(UiState::ENGINEERING); break;
        case UiState::ABOUT: if (inRect(x,y,330,260,120,45)) gotoState(UiState::UTILITY); break;
        case UiState::KEYBOARD: handleKeyboard(d,x,y); break;
    }
}

template <typename D> void mainMenu(D &d) { state=UiState::MAIN; previousState=UiState::MAIN; touchWasDown=false; uiDirty=true; drawCurrent(d); }

template <typename D> void uiLoop(D &d) {
    if (uiDirty) drawCurrent(d);
    uint16_t x=0,y=0;
    if (touchPressed(d,x,y)) handlePressed(d,x,y);
    if (state == UiState::WIFI_SCAN && networkScanCount() == WIFI_SCAN_RUNNING) { static uint32_t last=0; if(millis()-last>250){last=millis();uiDirty=true;} }
}

} // namespace tului
