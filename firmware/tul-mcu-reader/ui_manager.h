#pragma once

#include <Arduino.h>
#include <LovyanGFX.hpp>
#include "network_manager.h"
#include "settings.h"
#include "storage_manager.h"

namespace tului {

enum class UiState : uint8_t {
    MAIN, UTILITY, SYSTEM_INFO, DISPLAY, NETWORK, STORAGE,
    ENGINEERING, ABOUT, WIFI_SCAN, KEYBOARD, FORMAT_CONFIRM
};

enum class KeyboardMode : uint8_t { SSID, PASSWORD };

static UiState state = UiState::MAIN;
static UiState previousState = UiState::MAIN;
static KeyboardMode keyboardMode = KeyboardMode::SSID;
static bool uiDirty = true;
static bool touchWasDown = false;
static bool formatRunning = false;
static bool scanRunning = false;
static String kbValue;
static String selectedSsid;
static bool kbShift = true;
static bool kbPassword = false;

static void header(auto &d, const char *title) {
    d.fillScreen(TFT_BLACK);
    d.setTextSize(2);
    d.setTextColor(TFT_CYAN);
    d.setCursor(18, 12);
    d.print(title);
    d.drawFastHLine(18, 36, 444, TFT_DARKCYAN);
}

static void button(auto &d, int x, int y, int w, int h, const char *label,
                   uint16_t color = TFT_CYAN) {
    d.drawRoundRect(x, y, w, h, 6, color);
    d.setTextColor(TFT_WHITE);
    d.setTextSize(2);
    d.setCursor(x + 10, y + (h - 16) / 2);
    d.print(label);
}

static bool hit(auto &d, int x, int y, int w, int h, uint16_t *tx = nullptr,
                uint16_t *ty = nullptr) {
    uint16_t px = 0, py = 0;
    if (!d.getTouch(&px, &py)) return false;
    if (tx) *tx = px;
    if (ty) *ty = py;
    return px >= x && px < x + w && py >= y && py < y + h;
}

static bool pressed(auto &d, uint16_t *tx = nullptr, uint16_t *ty = nullptr) {
    uint16_t x = 0, y = 0;
    const bool down = d.getTouch(&x, &y);
    const bool edge = down && !touchWasDown;
    touchWasDown = down;
    if (edge) {
        if (tx) *tx = x;
        if (ty) *ty = y;
    }
    return edge;
}

static void gotoState(UiState next) {
    if (state != next) {
        previousState = state;
        state = next;
        uiDirty = true;
    }
}

static void drawSystemInfo(auto &d) {
    header(d, "TUL SYSTEM INFO");
    d.setTextColor(TFT_WHITE);
    d.setCursor(18, 52); d.printf("CPU       ESP32-S3");
    d.setCursor(18, 76); d.printf("RAM FREE  %lu KB", ESP.getFreeHeap() / 1024UL);
    d.setCursor(18, 100); d.printf("PSRAM     %lu / %lu KB", ESP.getFreePsram() / 1024UL, ESP.getPsramSize() / 1024UL);
    d.setCursor(18, 124); d.printf("FLASH     %lu MB", ESP.getFlashChipSize() / (1024UL * 1024UL));
    d.setCursor(18, 148); d.printf("SD CARD   %llu MB", storageCapacityBytes() / (1024ULL * 1024ULL));
    d.setCursor(18, 172); d.printf("SD FREE   %llu MB", storageFreeBytes() / (1024ULL * 1024ULL));
    d.setCursor(18, 196); d.printf("WiFi      %s", networkSSID());
    d.setCursor(18, 220); d.printf("IP        %s", networkIP());
    d.setCursor(18, 244); d.printf("RSSI      %d dBm", networkRSSI());
    button(d, 330, 260, 120, 45, "BACK");
}

static void drawDisplay(auto &d) {
    header(d, "DISPLAY");
    d.setTextColor(TFT_WHITE);
    d.setCursor(18, 55); d.printf("Brightness: %u", settings().brightness);
    button(d, 20, 82, 100, 42, "-");
    button(d, 135, 82, 100, 42, "+");
    button(d, 250, 82, 100, 42, "100%");
    d.setCursor(18, 145); d.printf("Screensaver: %u sec", settings().screensaverSeconds);
    button(d, 20, 172, 100, 42, "1 MIN");
    button(d, 135, 172, 100, 42, "5 MIN");
    button(d, 250, 172, 100, 42, "OFF");
    button(d, 330, 260, 120, 45, "BACK");
}

static void drawStorage(auto &d) {
    header(d, "STORAGE");
    d.setTextColor(TFT_WHITE);
    d.setCursor(18, 55); d.printf("Capacity: %llu MB", storageCapacityBytes() / (1024ULL * 1024ULL));
    d.setCursor(18, 79); d.printf("Free:     %llu MB", storageFreeBytes() / (1024ULL * 1024ULL));
    button(d, 18, 112, 200, 48, "FORMAT SD");
    button(d, 235, 112, 200, 48, "REFRESH");
    button(d, 330, 260, 120, 45, "BACK");
}

static void drawNetwork(auto &d) {
    header(d, "NETWORK");
    d.setTextColor(TFT_WHITE);
    d.setCursor(18, 52); d.printf("WiFi: %s", settings().wifiEnabled ? "ON" : "OFF");
    d.setCursor(18, 76); d.printf("SSID: %s", networkSSID());
    d.setCursor(18, 100); d.printf("IP: %s", networkIP());
    button(d, 18, 125, 130, 44, "WIFI ON/OFF");
    button(d, 160, 125, 130, 44, "SCAN");
    button(d, 302, 125, 130, 44, "BLE ON/OFF");
    button(d, 18, 185, 130, 44, "KEYBOARD");
    button(d, 160, 185, 130, 44, "SAVED");
    button(d, 330, 260, 120, 45, "BACK");
}

static void drawEngineering(auto &d) {
    header(d, "ENGINEERING MODE");
    d.setTextColor(TFT_WHITE);
    d.setCursor(18, 55); d.printf("RAM   %lu KB free", ESP.getFreeHeap() / 1024UL);
    d.setCursor(18, 79); d.printf("PSRAM %lu KB free", ESP.getFreePsram() / 1024UL);
    d.setCursor(18, 103); d.printf("SD    %s", storageCapacityBytes() ? "READY" : "FAIL");
    d.setCursor(18, 127); d.printf("WiFi  %s", settings().wifiEnabled ? "ENABLED" : "OFF");
    d.setCursor(18, 151); d.printf("BLE   %s", settings().bleEnabled ? "ENABLED" : "OFF");
    button(d, 18, 185, 130, 44, "TOUCH TEST");
    button(d, 160, 185, 130, 44, "SD TEST");
    button(d, 302, 185, 130, 44, "GPIO TEST");
    button(d, 330, 260, 120, 45, "BACK");
}

static void drawUtility(auto &d) {
    header(d, "TUL UTILITY");
    button(d, 18, 50, 135, 42, "SYSTEM INFO");
    button(d, 165, 50, 135, 42, "DISPLAY");
    button(d, 312, 50, 135, 42, "NETWORK");
    button(d, 18, 105, 135, 42, "STORAGE");
    button(d, 165, 105, 135, 42, "ENGINEER");
    button(d, 312, 105, 135, 42, "ABOUT TUL");
    button(d, 330, 260, 120, 45, "BACK");
}

static void drawMain(auto &d) {
    header(d, "TUL MCU FIRMWARE READER");
    button(d, 35, 70, 190, 70, "MCU READER");
    button(d, 255, 70, 190, 70, "UTILITY");
    button(d, 35, 165, 190, 70, "ENGINEERING");
    button(d, 255, 165, 190, 70, "SYSTEM INFO");
    d.setTextColor(TFT_GREEN); d.setCursor(35, 250); d.print("SD READY");
    d.setTextColor(TFT_CYAN); d.setCursor(255, 250); d.print("TUL SYSTEM");
}

static void drawAbout(auto &d) {
    header(d, "ABOUT TUL");
    d.setTextColor(TFT_WHITE);
    d.setCursor(18, 65); d.print("TUL MCU Firmware Reader");
    d.setCursor(18, 95); d.print("Standalone Service Instrument");
    d.setCursor(18, 125); d.print("Non-blocking UI architecture");
    button(d, 330, 260, 120, 45, "BACK");
}

static void drawScan(auto &d) {
    header(d, "WIFI SCAN");
    const int count = networkScanCount();
    const int shown = count > 7 ? 7 : (count > 0 ? count : 0);
    d.setTextColor(TFT_WHITE);
    d.setCursor(18, 52); d.printf("Found: %d", count < 0 ? 0 : count);
    for (int i = 0; i < shown; ++i) {
        d.setCursor(18, 76 + i * 26);
        d.printf("%d  %-20s %d dBm", i + 1, networkScanName(i).c_str(), networkScanRSSI(i));
    }
    button(d, 330, 260, 120, 45, "BACK");
}

static void drawFormatConfirm(auto &d) {
    header(d, "FORMAT SD");
    d.setTextColor(TFT_RED); d.setCursor(25, 65); d.print("ALL DATA WILL BE ERASED");
    d.setTextColor(TFT_WHITE); d.setCursor(25, 95); d.print("LOCAL ONLY - CONFIRM?");
    button(d, 25, 140, 190, 55, "CANCEL");
    button(d, 245, 140, 190, 55, "FORMAT", TFT_RED);
}

static void drawKeyboard(auto &d) {
    header(d, kbPassword ? "WIFI PASSWORD" : "WIFI SSID");
    d.setTextColor(TFT_WHITE); d.setCursor(12, 42);
    if (kbPassword) {
        for (size_t i = 0; i < kbValue.length(); ++i) d.print('*');
    } else d.print(kbValue);

    const char *lower = "qwertyuiopasdfghjklzxcvbnm";
    const char *upper = "QWERTYUIOPASDFGHJKLZXCVBNM";
    for (int i = 0; i < 26; ++i) {
        const int row = i / 10;
        const int col = i % 10;
        const int x = 8 + col * 46;
        const int y = 70 + row * 38;
        d.drawRect(x, y, 40, 32, TFT_CYAN);
        d.setCursor(x + 13, y + 8);
        d.print(kbShift ? upper[i] : lower[i]);
    }
    for (int i = 0; i < 10; ++i) {
        const int x = 8 + i * 46;
        d.drawRect(x, 188, 40, 32, TFT_CYAN);
        d.setCursor(x + 13, 196); d.print(i == 9 ? '0' : char('1' + i));
    }
    button(d, 8, 230, 90, 40, "SHIFT", TFT_YELLOW);
    button(d, 108, 230, 170, 40, "SPACE");
    button(d, 288, 230, 90, 40, "BKSP");
    button(d, 388, 230, 80, 40, "ENTER", TFT_GREEN);
}

static void drawCurrent(auto &d) {
    switch (state) {
        case UiState::MAIN: drawMain(d); break;
        case UiState::UTILITY: drawUtility(d); break;
        case UiState::SYSTEM_INFO: drawSystemInfo(d); break;
        case UiState::DISPLAY: drawDisplay(d); break;
        case UiState::NETWORK: drawNetwork(d); break;
        case UiState::STORAGE: drawStorage(d); break;
        case UiState::ENGINEERING: drawEngineering(d); break;
        case UiState::ABOUT: drawAbout(d); break;
        case UiState::WIFI_SCAN: drawScan(d); break;
        case UiState::FORMAT_CONFIRM: drawFormatConfirm(d); break;
        case UiState::KEYBOARD: drawKeyboard(d); break;
    }
    uiDirty = false;
}

static void startKeyboard(KeyboardMode mode, bool password, const String &initial = String()) {
    keyboardMode = mode;
    kbPassword = password;
    kbShift = true;
    kbValue = initial;
    kbValue.reserve(64);
    gotoState(UiState::KEYBOARD);
}

static void handleKeyboard(auto &d, uint16_t x, uint16_t y) {
    if (y >= 70 && y < 184) {
        const int row = (y - 70) / 38;
        const int col = x / 46;
        const int index = row * 10 + col;
        if (index >= 0 && index < 26 && col < 10) {
            const char *lower = "qwertyuiopasdfghjklzxcvbnm";
            const char *upper = "QWERTYUIOPASDFGHJKLZXCVBNM";
            kbValue += kbShift ? upper[index] : lower[index];
        }
    } else if (y >= 188 && y < 222 && x < 460) {
        const int n = x / 46;
        kbValue += n == 9 ? '0' : char('1' + n);
    } else if (y >= 230 && y < 275) {
        if (x < 100) kbShift = !kbShift;
        else if (x < 288) kbValue += ' ';
        else if (x < 388 && kbValue.length()) kbValue.remove(kbValue.length() - 1);
        else {
            if (keyboardMode == KeyboardMode::SSID) {
                selectedSsid = kbValue;
                startKeyboard(KeyboardMode::PASSWORD, true);
            } else {
                networkSaveCredentials(selectedSsid, kbValue);
                networkConnectSaved();
                gotoState(UiState::NETWORK);
            }
        }
        uiDirty = true;
    }
}

static void handlePressed(auto &d, uint16_t x, uint16_t y) {
    switch (state) {
        case UiState::MAIN:
            if (hit(d, 255, 70, 190, 70)) gotoState(UiState::UTILITY);
            else if (hit(d, 35, 165, 190, 70)) gotoState(UiState::ENGINEERING);
            else if (hit(d, 255, 165, 190, 70)) gotoState(UiState::SYSTEM_INFO);
            else if (hit(d, 35, 70, 190, 70)) { /* MCU reader placeholder */ }
            break;
        case UiState::UTILITY:
            if (hit(d, 18, 50, 135, 42)) gotoState(UiState::SYSTEM_INFO);
            else if (hit(d, 165, 50, 135, 42)) gotoState(UiState::DISPLAY);
            else if (hit(d, 312, 50, 135, 42)) gotoState(UiState::NETWORK);
            else if (hit(d, 18, 105, 135, 42)) gotoState(UiState::STORAGE);
            else if (hit(d, 165, 105, 135, 42)) gotoState(UiState::ENGINEERING);
            else if (hit(d, 312, 105, 135, 42)) gotoState(UiState::ABOUT);
            else if (hit(d, 330, 260, 120, 45)) gotoState(UiState::MAIN);
            break;
        case UiState::SYSTEM_INFO:
            if (hit(d, 330, 260, 120, 45)) gotoState(previousState == UiState::SYSTEM_INFO ? UiState::MAIN : previousState);
            break;
        case UiState::DISPLAY:
            if (hit(d, 20, 82, 100, 42)) { settings().brightness = settings().brightness > 20 ? settings().brightness - 20 : 0; analogWrite(45, settings().brightness); settingsSave(); uiDirty = true; }
            else if (hit(d, 135, 82, 100, 42)) { settings().brightness = settings().brightness < 235 ? settings().brightness + 20 : 255; analogWrite(45, settings().brightness); settingsSave(); uiDirty = true; }
            else if (hit(d, 250, 82, 100, 42)) { settings().brightness = 255; analogWrite(45, 255); settingsSave(); uiDirty = true; }
            else if (hit(d, 20, 172, 100, 42)) { settings().screensaverSeconds = 60; settingsSave(); uiDirty = true; }
            else if (hit(d, 135, 172, 100, 42)) { settings().screensaverSeconds = 300; settingsSave(); uiDirty = true; }
            else if (hit(d, 250, 172, 100, 42)) { settings().screensaverSeconds = 0; settingsSave(); uiDirty = true; }
            else if (hit(d, 330, 260, 120, 45)) gotoState(UiState::UTILITY);
            break;
        case UiState::STORAGE:
            if (hit(d, 18, 112, 200, 48)) gotoState(UiState::FORMAT_CONFIRM);
            else if (hit(d, 235, 112, 200, 48)) { storageBegin(); uiDirty = true; }
            else if (hit(d, 330, 260, 120, 45)) gotoState(UiState::UTILITY);
            break;
        case UiState::FORMAT_CONFIRM:
            if (hit(d, 25, 140, 190, 55)) gotoState(UiState::STORAGE);
            else if (hit(d, 245, 140, 190, 55)) {
                d.fillScreen(TFT_BLACK); d.setTextColor(TFT_YELLOW); d.setCursor(25, 120); d.print("FORMATTING...");
                formatRunning = true;
                const bool ok = storageFormat();
                formatRunning = false;
                d.setTextColor(ok ? TFT_GREEN : TFT_RED); d.setCursor(25, 160); d.print(ok ? "FORMAT COMPLETE" : "FORMAT FAILED");
                delay(900);
                gotoState(UiState::STORAGE);
            }
            break;
        case UiState::NETWORK:
            if (hit(d, 18, 125, 130, 44)) { networkToggleWiFi(); uiDirty = true; }
            else if (hit(d, 160, 125, 130, 44)) {
                scanRunning = true; gotoState(UiState::WIFI_SCAN); networkScanWiFi(); scanRunning = false; uiDirty = true;
            }
            else if (hit(d, 302, 125, 130, 44)) { networkToggleBLE(); uiDirty = true; }
            else if (hit(d, 18, 185, 130, 44)) startKeyboard(KeyboardMode::SSID, false);
            else if (hit(d, 160, 185, 130, 44)) { networkConnectSaved(); uiDirty = true; }
            else if (hit(d, 330, 260, 120, 45)) gotoState(UiState::UTILITY);
            break;
        case UiState::WIFI_SCAN: {
            if (hit(d, 330, 260, 120, 45)) { gotoState(UiState::NETWORK); break; }
            const int count = networkScanCount();
            const int shown = count > 7 ? 7 : (count > 0 ? count : 0);
            for (int i = 0; i < shown; ++i) {
                if (hit(d, 10, 70 + i * 26, 450, 25)) {
                    selectedSsid = networkScanName(i);
                    startKeyboard(KeyboardMode::PASSWORD, true);
                    break;
                }
            }
            break;
        }
        case UiState::ENGINEERING:
            if (hit(d, 330, 260, 120, 45)) gotoState(previousState == UiState::ENGINEERING ? UiState::MAIN : previousState);
            else if (hit(d, 18, 185, 130, 44)) { uiDirty = true; }
            else if (hit(d, 160, 185, 130, 44)) { storageBegin(); uiDirty = true; }
            else if (hit(d, 302, 185, 130, 44)) { uiDirty = true; }
            break;
        case UiState::ABOUT:
            if (hit(d, 330, 260, 120, 45)) gotoState(UiState::UTILITY);
            break;
        case UiState::KEYBOARD:
            handleKeyboard(d, x, y);
            break;
    }
}

template <typename Display>
void mainMenu(Display &d) {
    state = UiState::MAIN;
    previousState = UiState::MAIN;
    touchWasDown = false;
    uiDirty = true;
    drawCurrent(d);
}

template <typename Display>
void uiLoop(Display &d) {
    if (uiDirty) drawCurrent(d);
    uint16_t x = 0, y = 0;
    if (pressed(d, &x, &y)) handlePressed(d, x, y);
}

} // namespace tului
