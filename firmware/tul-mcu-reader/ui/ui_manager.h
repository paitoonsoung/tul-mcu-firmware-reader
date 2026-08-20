#pragma once

#include <Arduino.h>
#include <LovyanGFX.hpp>
#include <WiFi.h>
#include "network_manager.h"
#include "settings.h"
#include "storage_manager.h"
#include "board_config.h"
#include "atmega328p_isp.h"

namespace tului {

enum class UiState : uint8_t { MAIN, UTILITY, SYSTEM_INFO, DISPLAY_SCREEN, NETWORK, STORAGE, ENGINEERING, ABOUT, WIFI_SCAN, KEYBOARD, FORMAT_CONFIRM, MCU_READER, TOUCH_TEST, SD_TEST, GPIO_TEST };
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
static uint8_t lastSignature[3] = {0, 0, 0};
static bool lastMcuOk = false;
static int lastScanCount = -99;

static constexpr uint16_t C_BLUE = 0x2D7F;
static constexpr uint16_t C_GREEN = 0x05E0;
static constexpr uint16_t C_ORANGE = 0xFD20;
static constexpr uint16_t C_PURPLE = 0x801F;
static constexpr uint16_t C_RED = 0xF800;
static constexpr uint16_t C_YELLOW = 0xFFE0;
static constexpr uint16_t C_TEAL = 0x05FF;
static constexpr uint16_t C_GRAY = 0x7BEF;

static void gotoState(UiState next) { if (state != next) { previousState = state; state = next; uiDirty = true; } }

template <typename Display>
static void header(Display &d, const char *title) {
    d.fillScreen(TFT_BLACK);
    d.setTextSize(2);
    d.setTextColor(TFT_CYAN);
    d.setCursor(12, 10);
    d.print(title);
    d.drawFastHLine(12, 34, 456, C_TEAL);
}

template <typename Display>
static void button(Display &d, int x, int y, int w, int h, const char *label, uint16_t color = C_BLUE, uint8_t textSize = 2) {
    d.fillRoundRect(x, y, w, h, 7, color);
    d.drawRoundRect(x, y, w, h, 7, TFT_WHITE);
    d.setTextColor(TFT_WHITE);
    d.setTextSize(textSize);
    const int tw = strlen(label) * 6 * textSize;
    d.setCursor(x + ((w - tw) > 5 ? (w - tw) / 2 : 5), y + ((h - 8 * textSize) > 4 ? (h - 8 * textSize) / 2 : 4));
    d.print(label);
}

template <typename Display>
static bool hit(Display &, int x, int y, int w, int h, uint16_t px, uint16_t py) { return px >= x && px < x + w && py >= y && py < y + h; }

template <typename Display>
static bool pressed(Display &d, uint16_t *tx = nullptr, uint16_t *ty = nullptr) {
    uint16_t x = 0, y = 0;
    const bool down = d.getTouch(&x, &y);
    const bool edge = down && !touchWasDown;
    touchWasDown = down;
    if (edge) { if (tx) *tx = x; if (ty) *ty = y; }
    return edge;
}

template <typename Display>
static void drawMain(Display &d) {
    header(d, "TUL MCU FIRMWARE READER");
    button(d, 25, 55, 205, 70, "MCU READER", C_PURPLE);
    button(d, 250, 55, 205, 70, "UTILITY", C_BLUE);
    button(d, 25, 145, 205, 70, "ENGINEERING", C_ORANGE);
    button(d, 250, 145, 205, 70, "SYSTEM INFO", C_TEAL, 1);
    d.setTextSize(2); d.setTextColor(C_GREEN); d.setCursor(30, 245); d.print("SD READY");
    d.setTextColor(C_YELLOW); d.setCursor(250, 245); d.print("TUL SERVICE TOOL");
}

template <typename Display>
static void drawUtility(Display &d) {
    header(d, "TUL UTILITY");
    button(d, 15, 48, 140, 45, "SYS INFO", C_TEAL);
    button(d, 165, 48, 140, 45, "DISPLAY", C_PURPLE);
    button(d, 315, 48, 140, 45, "NETWORK", C_BLUE);
    button(d, 15, 108, 140, 45, "STORAGE", C_GREEN);
    button(d, 165, 108, 140, 45, "ENGINEER", C_ORANGE);
    button(d, 315, 108, 140, 45, "ABOUT TUL", C_TEAL);
    button(d, 335, 255, 120, 42, "BACK", C_RED);
}

template <typename Display>
static void drawSystemInfo(Display &d) {
    header(d, "TUL SYSTEM INFO"); d.setTextColor(TFT_WHITE); d.setTextSize(2);
    d.setCursor(15, 48); d.printf("CPU      ESP32-S3");
    d.setCursor(15, 72); d.printf("RAM      %lu KB", ESP.getFreeHeap() / 1024UL);
    d.setCursor(15, 96); d.printf("PSRAM    %lu / %lu KB", ESP.getFreePsram() / 1024UL, ESP.getPsramSize() / 1024UL);
    d.setCursor(15, 120); d.printf("FLASH    %lu MB", ESP.getFlashChipSize() / (1024UL * 1024UL));
    d.setCursor(15, 144); d.printf("SD       %llu MB", storageCapacityBytes() / (1024ULL * 1024ULL));
    d.setCursor(15, 168); d.printf("SD FREE  %llu MB", storageFreeBytes() / (1024ULL * 1024ULL));
    d.setCursor(15, 192); d.printf("WIFI     %.25s", networkSSID());
    d.setCursor(15, 216); d.printf("IP       %s", networkIP());
    d.setCursor(15, 240); d.printf("RSSI     %d dBm", networkRSSI());
    button(d, 335, 255, 120, 42, "BACK", C_RED);
}

template <typename Display>
static void drawDisplay(Display &d) {
    header(d, "DISPLAY"); d.setTextColor(TFT_WHITE); d.setTextSize(2);
    d.setCursor(15, 52); d.printf("Brightness: %u", settings().brightness);
    button(d, 20, 80, 100, 42, "-", C_RED); button(d, 135, 80, 100, 42, "+", C_GREEN); button(d, 250, 80, 100, 42, "100%", C_TEAL);
    d.setCursor(15, 145); d.printf("Screensaver: %u sec", settings().screensaverSeconds);
    button(d, 20, 172, 100, 42, "1 MIN", C_BLUE); button(d, 135, 172, 100, 42, "5 MIN", C_PURPLE); button(d, 250, 172, 100, 42, "OFF", C_RED);
    button(d, 335, 255, 120, 42, "BACK", C_RED);
}

template <typename Display>
static void drawStorage(Display &d) {
    header(d, "STORAGE"); d.setTextColor(TFT_WHITE); d.setTextSize(2);
    d.setCursor(15, 52); d.printf("Capacity: %llu MB", storageCapacityBytes() / (1024ULL * 1024ULL));
    d.setCursor(15, 78); d.printf("Free:     %llu MB", storageFreeBytes() / (1024ULL * 1024ULL));
    button(d, 15, 112, 205, 48, "FORMAT SD", C_RED); button(d, 235, 112, 205, 48, "REFRESH", C_GREEN); button(d, 335, 255, 120, 42, "BACK", C_RED);
}

template <typename Display>
static void drawNetwork(Display &d) {
    header(d, "NETWORK"); d.setTextColor(TFT_WHITE); d.setTextSize(2);
    d.setCursor(15, 48); d.printf("WiFi: %s", settings().wifiEnabled ? "ON" : "OFF");
    d.setCursor(15, 72); d.printf("SSID: %.25s", networkSSID()); d.setCursor(15, 96); d.printf("IP: %s", networkIP());
    button(d, 15, 120, 135, 45, "WIFI ON/OFF", C_BLUE, 1); button(d, 160, 120, 135, 45, "SCAN", C_GREEN); button(d, 305, 120, 135, 45, "BLE ON/OFF", C_PURPLE, 1);
    button(d, 15, 180, 135, 45, "KEYBOARD", C_ORANGE, 1); button(d, 160, 180, 135, 45, "SAVED", C_TEAL); button(d, 335, 255, 120, 42, "BACK", C_RED);
}

template <typename Display>
static void drawEngineering(Display &d) {
    header(d, "ENGINEERING MODE"); d.setTextColor(TFT_WHITE); d.setTextSize(2);
    d.setCursor(15, 48); d.printf("RAM  %lu KB", ESP.getFreeHeap() / 1024UL); d.setCursor(15, 72); d.printf("PSRAM %lu KB", ESP.getFreePsram() / 1024UL);
    d.setCursor(15, 96); d.printf("SD   %s", storageCapacityBytes() ? "READY" : "FAIL"); d.setCursor(15, 120); d.printf("WIFI %s", settings().wifiEnabled ? "ON" : "OFF"); d.setCursor(15, 144); d.printf("BLE  %s", settings().bleEnabled ? "ON" : "OFF");
    button(d, 15, 180, 135, 45, "TOUCH TEST", C_BLUE, 1); button(d, 160, 180, 135, 45, "SD TEST", C_GREEN); button(d, 305, 180, 135, 45, "GPIO TEST", C_ORANGE, 1); button(d, 335, 255, 120, 42, "BACK", C_RED);
}

template <typename Display>
static void drawAbout(Display &d) { header(d, "ABOUT TUL"); d.setTextColor(TFT_WHITE); d.setTextSize(2); d.setCursor(15, 62); d.print("TUL MCU Firmware Reader"); d.setCursor(15, 92); d.print("Service / Recovery Tool"); d.setCursor(15, 122); d.print("ESP32-S3 platform"); button(d, 335, 255, 120, 42, "BACK", C_RED); }

template <typename Display>
static void drawScan(Display &d) {
    header(d, "WIFI SCAN"); const int count = networkScanCount(); d.setTextColor(TFT_WHITE); d.setTextSize(2);
    if (count == WIFI_SCAN_RUNNING) { d.setTextColor(C_YELLOW); d.setCursor(15, 60); d.print("SCANNING..."); }
    else if (count < 0) { d.setTextColor(C_RED); d.setCursor(15, 60); d.print("SCAN FAILED"); }
    else { d.setCursor(15, 55); d.printf("Found: %d", count); const int shown = count > 6 ? 6 : count; for (int i = 0; i < shown; ++i) { d.setCursor(15, 82 + i * 27); d.printf("%d %-22s %d", i + 1, networkScanName(i).c_str(), networkScanRSSI(i)); } }
    button(d, 335, 255, 120, 42, "BACK", C_RED);
}

template <typename Display>
static void drawMcuReader(Display &d) {
    header(d, "MCU READER"); d.setTextColor(TFT_WHITE); d.setTextSize(2); d.setCursor(15, 55); d.print("Target: ATmega328P"); d.setCursor(15, 82); d.printf("SIGNATURE: %02X %02X %02X", lastSignature[0], lastSignature[1], lastSignature[2]); d.setTextColor(lastMcuOk ? C_GREEN : C_YELLOW); d.setCursor(15, 112); d.print(lastMcuOk ? "ISP READY" : "PRESS READ"); button(d, 15, 155, 190, 55, "READ SIGNATURE", C_PURPLE, 1); button(d, 225, 155, 190, 55, "READ FLASH", C_BLUE, 1); button(d, 335, 255, 120, 42, "BACK", C_RED);
}

template <typename Display>
static void drawTest(Display &d, const char *title) {
    header(d, title); d.setTextColor(TFT_WHITE); d.setTextSize(2); d.setCursor(15, 60); d.print(testMessage.length() ? testMessage : "READY");
    if (state == UiState::TOUCH_TEST) { d.drawRect(15, 100, 425, 120, C_TEAL); d.setCursor(25, 115); d.print("Touch inside the box"); }
    else if (state == UiState::GPIO_TEST) { d.setCursor(15, 105); d.printf("ISP MOSI: GPIO %u", PIN_ISP_MOSI); d.setCursor(15, 130); d.printf("ISP MISO: GPIO %u", PIN_ISP_MISO); d.setCursor(15, 155); d.printf("ISP SCK : GPIO %u", PIN_ISP_SCK); d.setCursor(15, 180); d.printf("RESET   : GPIO %u", PIN_TARGET_RESET); }
    else if (state == UiState::SD_TEST) { d.setCursor(15, 105); d.print("SD interface test / status"); }
    button(d, 335, 255, 120, 42, "BACK", C_RED);
}

template <typename Display>
static void drawFormatConfirm(Display &d) { header(d, "FORMAT SD"); d.setTextColor(C_RED); d.setCursor(25, 65); d.print("ALL DATA WILL BE ERASED"); d.setTextColor(TFT_WHITE); d.setCursor(25, 95); d.print("LOCAL ONLY - CONFIRM?"); button(d, 25, 140, 190, 55, "CANCEL", C_GRAY); button(d, 245, 140, 190, 55, "FORMAT", C_RED); }

template <typename Display>
static void drawCurrent(Display &d) {
    switch (state) {
        case UiState::MAIN: drawMain(d); break; case UiState::UTILITY: drawUtility(d); break; case UiState::SYSTEM_INFO: drawSystemInfo(d); break; case UiState::DISPLAY_SCREEN: drawDisplay(d); break; case UiState::NETWORK: drawNetwork(d); break; case UiState::STORAGE: drawStorage(d); break; case UiState::ENGINEERING: drawEngineering(d); break; case UiState::ABOUT: drawAbout(d); break; case UiState::WIFI_SCAN: drawScan(d); break; case UiState::MCU_READER: drawMcuReader(d); break; case UiState::TOUCH_TEST: drawTest(d, "TOUCH TEST"); break; case UiState::SD_TEST: drawTest(d, "SD CARD TEST"); break; case UiState::GPIO_TEST: drawTest(d, "GPIO TEST"); break; case UiState::FORMAT_CONFIRM: drawFormatConfirm(d); break; case UiState::KEYBOARD: break;
    }
    uiDirty = false;
}

static void startKeyboard(KeyboardMode mode, bool password, const String &initial = String()) { keyboardMode = mode; kbPassword = password; kbShift = true; kbValue = initial; kbValue.reserve(64); gotoState(UiState::KEYBOARD); }

template <typename Display>
static void handleKeyboard(Display &, uint16_t x, uint16_t y) {
    if (y >= 70 && y < 184) { const int row = (y - 70) / 38; const int col = x / 46; const int index = row * 10 + col; if (index >= 0 && index < 26 && col < 10) { const char *lower = "qwertyuiopasdfghjklzxcvbnm"; const char *upper = "QWERTYUIOPASDFGHJKLZXCVBNM"; kbValue += kbShift ? upper[index] : lower[index]; } }
    else if (y >= 188 && y < 222 && x < 460) { const int n = x / 46; kbValue += n == 9 ? '0' : char('1' + n); }
    else if (y >= 230 && y < 275) { if (x < 100) kbShift = !kbShift; else if (x < 288) kbValue += ' '; else if (x < 388) { if (kbValue.length()) kbValue.remove(kbValue.length() - 1); } else if (keyboardMode == KeyboardMode::SSID) { selectedSsid = kbValue; startKeyboard(KeyboardMode::PASSWORD, true); } else { networkSaveCredentials(selectedSsid, kbValue); networkConnectSaved(); gotoState(UiState::NETWORK); } uiDirty = true; }
}

template <typename Display>
static void handlePressed(Display &d, uint16_t x, uint16_t y) {
    switch (state) {
        case UiState::MAIN: if (hit(d,25,55,205,70,x,y)) gotoState(UiState::MCU_READER); else if (hit(d,250,55,205,70,x,y)) gotoState(UiState::UTILITY); else if (hit(d,25,145,205,70,x,y)) gotoState(UiState::ENGINEERING); else if (hit(d,250,145,205,70,x,y)) gotoState(UiState::SYSTEM_INFO); break;
        case UiState::UTILITY: if (hit(d,15,48,140,45,x,y)) gotoState(UiState::SYSTEM_INFO); else if (hit(d,165,48,140,45,x,y)) gotoState(UiState::DISPLAY_SCREEN); else if (hit(d,315,48,140,45,x,y)) gotoState(UiState::NETWORK); else if (hit(d,15,108,140,45,x,y)) gotoState(UiState::STORAGE); else if (hit(d,165,108,140,45,x,y)) gotoState(UiState::ENGINEERING); else if (hit(d,315,108,140,45,x,y)) gotoState(UiState::ABOUT); else if (hit(d,335,255,120,42,x,y)) gotoState(UiState::MAIN); break;
        case UiState::SYSTEM_INFO: if (hit(d,335,255,120,42,x,y)) gotoState(UiState::MAIN); break;
        case UiState::DISPLAY_SCREEN: if (hit(d,335,255,120,42,x,y)) gotoState(UiState::UTILITY); break;
        case UiState::STORAGE: if (hit(d,15,112,205,48,x,y)) gotoState(UiState::FORMAT_CONFIRM); else if (hit(d,235,112,205,48,x,y)) { storageBegin(); uiDirty = true; } else if (hit(d,335,255,120,42,x,y)) gotoState(UiState::UTILITY); break;
        case UiState::FORMAT_CONFIRM: if (hit(d,25,140,190,55,x,y)) gotoState(UiState::STORAGE); else if (hit(d,245,140,190,55,x,y)) { testMessage = "FORMAT REQUESTED"; gotoState(UiState::STORAGE); } break;
        case UiState::NETWORK: if (hit(d,15,120,135,45,x,y)) { networkToggleWiFi(); uiDirty = true; } else if (hit(d,160,120,135,45,x,y)) { networkScanWiFi(); lastScanCount = -99; gotoState(UiState::WIFI_SCAN); } else if (hit(d,305,120,135,45,x,y)) { networkToggleBLE(); uiDirty = true; } else if (hit(d,15,180,135,45,x,y)) startKeyboard(KeyboardMode::SSID,false); else if (hit(d,160,180,135,45,x,y)) { networkConnectSaved(); uiDirty = true; } else if (hit(d,335,255,120,42,x,y)) gotoState(UiState::UTILITY); break;
        case UiState::WIFI_SCAN: if (hit(d,335,255,120,42,x,y)) { WiFi.scanDelete(); gotoState(UiState::NETWORK); } break;
        case UiState::ENGINEERING: if (hit(d,15,180,135,45,x,y)) { testMessage="Touch test ACTIVE"; gotoState(UiState::TOUCH_TEST); } else if (hit(d,160,180,135,45,x,y)) { testMessage=storageCapacityBytes()?"SD TEST: CARD READY":"SD TEST: NO CARD"; gotoState(UiState::SD_TEST); } else if (hit(d,305,180,135,45,x,y)) { testMessage="GPIO TEST: ISP PINS READY"; gotoState(UiState::GPIO_TEST); } else if (hit(d,335,255,120,42,x,y)) gotoState(UiState::MAIN); break;
        case UiState::TOUCH_TEST: if (hit(d,15,100,425,120,x,y)) { testMessage="Touch OK: X="+String(x)+" Y="+String(y); uiDirty=true; } else if (hit(d,335,255,120,42,x,y)) gotoState(UiState::ENGINEERING); break;
        case UiState::SD_TEST: if (hit(d,335,255,120,42,x,y)) gotoState(UiState::ENGINEERING); break;
        case UiState::GPIO_TEST: if (hit(d,335,255,120,42,x,y)) gotoState(UiState::ENGINEERING); break;
        case UiState::MCU_READER: if (hit(d,15,155,190,55,x,y)) { Atmega328pIsp reader; reader.begin(); lastMcuOk=reader.readSignature(lastSignature); uiDirty=true; } else if (hit(d,335,255,120,42,x,y)) gotoState(UiState::MAIN); break;
        case UiState::ABOUT: if (hit(d,335,255,120,42,x,y)) gotoState(UiState::UTILITY); break;
        case UiState::KEYBOARD: handleKeyboard(d,x,y); break;
    }
}

template <typename Display>
void mainMenu(Display &d) { state=UiState::MAIN; previousState=UiState::MAIN; touchWasDown=false; uiDirty=true; drawCurrent(d); }

template <typename Display>
void uiLoop(Display &d) {
    if (state == UiState::WIFI_SCAN) { const int count=networkScanCount(); if (count != lastScanCount) { lastScanCount=count; uiDirty=true; } }
    if (uiDirty) drawCurrent(d);
    uint16_t x=0,y=0; if (pressed(d,&x,&y)) handlePressed(d,x,y);
}

} // namespace tului
