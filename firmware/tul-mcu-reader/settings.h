#pragma once

#include <Arduino.h>

struct TulSettings {
    uint8_t brightness = 200;
    uint16_t screensaverSeconds = 300;
    bool wifiEnabled = true;
    bool bleEnabled = false;
    bool autoConnect = true;
};

void settingsBegin();
TulSettings &settings();
void settingsSave();
