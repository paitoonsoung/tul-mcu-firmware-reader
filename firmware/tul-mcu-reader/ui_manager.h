#pragma once

#include <Arduino.h>
#include <LovyanGFX.hpp>
#include "network_manager.h"
#include "settings.h"
#include "storage_manager.h"

namespace tului {

template <typename Display>
void header(Display &d, const char *title) {
    d.fillScreen(TFT_BLACK);
    d.setTextSize(2);
    d.setTextColor(TFT_CYAN);
    d.setCursor(18, 12);
    d.print(title);
    d.drawFastHLine(18, 36, 444, TFT_DARKCYAN);
}

template <typename Display>
bool button(Display &d, int x, int y, int w, int h, const char *label,
            uint16_t color = TFT_CYAN) {
    d.drawRoundRect(x, y, w, h, 6, color);
    d.setTextColor(TFT_WHITE);
    d.setTextSize(2);
    d.setCursor(x + 10, y + (h - 16) / 2);
    d.print(label);
    return true;
}

template <typename Display>
bool touch(Display &d, int x, int y, int w, int h) {
    uint16_t tx = 0, ty = 0;
    if (!d.getTouch(&tx, &ty)) {
        return false;
    }
    return tx >= x && tx < x + w && ty >= y && ty < y + h;
}

template <typename Display>
void waitRelease(Display &d) {
    uint16_t x, y;
    while (d.getTouch(&x, &y)) {
        delay(10);
    }
}

template <typename Display>
void systemInfo(Display &d) {
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
    while (true) {
        if (touch(d, 330, 260, 120, 45)) { waitRelease(d); return; }
        networkLoop();
        delay(30);
    }
}

template <typename Display>
void displayMenu(Display &d) {
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
    while (true) {
        if (touch(d, 20, 82, 100, 42)) { settings().brightness = settings().brightness > 20 ? settings().brightness - 20 : 0; settingsSave(); }
        else if (touch(d, 135, 82, 100, 42)) { settings().brightness = settings().brightness < 235 ? settings().brightness + 20 : 255; settingsSave(); }
        else if (touch(d, 250, 82, 100, 42)) { settings().brightness = 255; settingsSave(); }
        else if (touch(d, 20, 172, 100, 42)) { settings().screensaverSeconds = 60; settingsSave(); }
        else if (touch(d, 135, 172, 100, 42)) { settings().screensaverSeconds = 300; settingsSave(); }
        else if (touch(d, 250, 172, 100, 42)) { settings().screensaverSeconds = 0; settingsSave(); }
        else if (touch(d, 330, 260, 120, 45)) { waitRelease(d); return; }
        waitRelease(d); delay(20);
        d.setCursor(18, 55); d.setTextColor(TFT_WHITE); d.printf("Brightness: %u   ", settings().brightness);
        d.setCursor(18, 145); d.printf("Screensaver: %u sec   ", settings().screensaverSeconds);
    }
}

template <typename Display>
void storageMenu(Display &d) {
    header(d, "STORAGE");
    d.setTextColor(TFT_WHITE);
    d.setCursor(18, 55); d.printf("Capacity: %llu MB", storageCapacityBytes() / (1024ULL * 1024ULL));
    d.setCursor(18, 79); d.printf("Free:     %llu MB", storageFreeBytes() / (1024ULL * 1024ULL));
    button(d, 18, 112, 200, 48, "FORMAT SD");
    button(d, 235, 112, 200, 48, "REFRESH");
    button(d, 330, 260, 120, 45, "BACK");
    while (true) {
        if (touch(d, 18, 112, 200, 48)) {
            waitRelease(d);
            header(d, "FORMAT SD");
            d.setTextColor(TFT_RED); d.setCursor(25, 65); d.print("ALL DATA WILL BE ERASED");
            d.setTextColor(TFT_WHITE); d.setCursor(25, 95); d.print("Confirm local operation?");
            button(d, 25, 140, 190, 55, "CANCEL");
            button(d, 245, 140, 190, 55, "FORMAT");
            while (true) {
                if (touch(d, 25, 140, 190, 55)) { waitRelease(d); break; }
                if (touch(d, 245, 140, 190, 55)) {
                    waitRelease(d); d.fillScreen(TFT_BLACK); d.setTextColor(TFT_YELLOW); d.setCursor(25, 120); d.print("FORMATTING...");
                    const bool ok = storageFormat();
                    d.setTextColor(ok ? TFT_GREEN : TFT_RED); d.setCursor(25, 160); d.print(ok ? "FORMAT COMPLETE" : "FORMAT FAILED");
                    delay(1800); break;
                }
                delay(20);
            }
            header(d, "STORAGE");
            d.setTextColor(TFT_WHITE); d.setCursor(18, 55); d.printf("Capacity: %llu MB", storageCapacityBytes() / (1024ULL * 1024ULL));
            d.setCursor(18, 79); d.printf("Free:     %llu MB", storageFreeBytes() / (1024ULL * 1024ULL));
            button(d, 18, 112, 200, 48, "FORMAT SD"); button(d, 235, 112, 200, 48, "REFRESH"); button(d, 330, 260, 120, 45, "BACK");
        } else if (touch(d, 235, 112, 200, 48)) {
            storageBegin(); waitRelease(d);
        } else if (touch(d, 330, 260, 120, 45)) { waitRelease(d); return; }
        delay(20);
    }
}

template <typename Display>
void networkMenu(Display &d) {
    header(d, "NETWORK");
    d.setTextColor(TFT_WHITE);
    d.setCursor(18, 52); d.printf("WiFi: %s", settings().wifiEnabled ? "ON" : "OFF");
    d.setCursor(18, 76); d.printf("SSID: %s", networkSSID());
    d.setCursor(18, 100); d.printf("IP:   %s", networkIP());
    button(d, 18, 125, 130, 44, "WIFI ON/OFF");
    button(d, 160, 125, 130, 44, "SCAN");
    button(d, 302, 125, 130, 44, "BLE ON/OFF");
    button(d, 18, 185, 130, 44, "SAVED");
    button(d, 330, 260, 120, 45, "BACK");
    while (true) {
        networkLoop();
        if (touch(d, 18, 125, 130, 44)) { networkToggleWiFi(); waitRelease(d); }
        else if (touch(d, 160, 125, 130, 44)) { networkScanWiFi(); waitRelease(d); }
        else if (touch(d, 302, 125, 130, 44)) { networkToggleBLE(); waitRelease(d); }
        else if (touch(d, 330, 260, 120, 45)) { waitRelease(d); return; }
        if (networkScanCount() >= 0) {
            const int n = networkScanCount();
            d.setTextColor(TFT_GREEN); d.setCursor(18, 235); d.printf("Networks found: %d", n);
        }
        delay(30);
    }
}

template <typename Display>
void engineeringMenu(Display &d) {
    header(d, "ENGINEERING MODE");
    d.setTextColor(TFT_WHITE);
    d.setCursor(18, 55); d.printf("RAM  %lu KB free", ESP.getFreeHeap() / 1024UL);
    d.setCursor(18, 79); d.printf("PSRAM %lu KB free", ESP.getFreePsram() / 1024UL);
    d.setCursor(18, 103); d.printf("SD %s", storageCapacityBytes() ? "READY" : "FAIL");
    d.setCursor(18, 127); d.printf("WiFi %s", settings().wifiEnabled ? "ENABLED" : "OFF");
    d.setCursor(18, 151); d.printf("BLE %s", settings().bleEnabled ? "ENABLED" : "OFF");
    button(d, 18, 185, 130, 44, "TOUCH TEST");
    button(d, 160, 185, 130, 44, "SD TEST");
    button(d, 302, 185, 130, 44, "GPIO TEST");
    button(d, 330, 260, 120, 45, "BACK");
    while (true) {
        if (touch(d, 330, 260, 120, 45)) { waitRelease(d); return; }
        if (touch(d, 18, 185, 130, 44)) { waitRelease(d); d.fillRect(18, 235, 280, 30, TFT_BLACK); d.setTextColor(TFT_GREEN); d.setCursor(18, 240); d.print("TOUCH OK"); }
        if (touch(d, 160, 185, 130, 44)) { waitRelease(d); const bool ok = storageBegin(); d.setTextColor(ok ? TFT_GREEN : TFT_RED); d.setCursor(18, 240); d.print(ok ? "SD OK" : "SD FAIL"); }
        delay(30);
    }
}

template <typename Display>
void utilityMenu(Display &d) {
    while (true) {
        header(d, "TUL UTILITY");
        button(d, 18, 50, 135, 42, "SYSTEM INFO");
        button(d, 165, 50, 135, 42, "DISPLAY");
        button(d, 312, 50, 135, 42, "NETWORK");
        button(d, 18, 105, 135, 42, "STORAGE");
        button(d, 165, 105, 135, 42, "ENGINEER");
        button(d, 312, 105, 135, 42, "ABOUT TUL");
        button(d, 330, 260, 120, 45, "BACK");
        while (true) {
            if (touch(d, 18, 50, 135, 42)) { waitRelease(d); systemInfo(d); break; }
            if (touch(d, 165, 50, 135, 42)) { waitRelease(d); displayMenu(d); break; }
            if (touch(d, 312, 50, 135, 42)) { waitRelease(d); networkMenu(d); break; }
            if (touch(d, 18, 105, 135, 42)) { waitRelease(d); storageMenu(d); break; }
            if (touch(d, 165, 105, 135, 42)) { waitRelease(d); engineeringMenu(d); break; }
            if (touch(d, 312, 105, 135, 42)) { waitRelease(d); header(d, "ABOUT TUL"); d.setTextColor(TFT_WHITE); d.setCursor(18, 65); d.print("TUL MCU Firmware Reader"); d.setCursor(18, 95); d.print("Standalone Service Instrument"); d.setCursor(18, 125); d.print("ESP32-S3 / WT32-SC01 Plus"); button(d, 330, 260, 120, 45, "BACK"); while (!touch(d,330,260,120,45)) delay(20); waitRelease(d); break; }
            if (touch(d, 330, 260, 120, 45)) { waitRelease(d); return; }
            delay(20);
        }
    }
}

template <typename Display>
void mainMenu(Display &d) {
    header(d, "TUL MCU FIRMWARE READER");
    button(d, 35, 70, 190, 70, "MCU READER");
    button(d, 255, 70, 190, 70, "UTILITY");
    button(d, 35, 165, 190, 70, "ENGINEERING");
    button(d, 255, 165, 190, 70, "SYSTEM INFO");
    d.setTextColor(TFT_GREEN); d.setCursor(35, 250); d.print("SD READY");
    d.setTextColor(TFT_CYAN); d.setCursor(255, 250); d.print("TUL SYSTEM");
    while (true) {
        if (touch(d, 255, 70, 190, 70)) { waitRelease(d); utilityMenu(d); return; }
        if (touch(d, 35, 165, 190, 70)) { waitRelease(d); engineeringMenu(d); return; }
        if (touch(d, 255, 165, 190, 70)) { waitRelease(d); systemInfo(d); return; }
        if (touch(d, 35, 70, 190, 70)) { waitRelease(d); return; }
        delay(20);
    }
}

} // namespace tului
