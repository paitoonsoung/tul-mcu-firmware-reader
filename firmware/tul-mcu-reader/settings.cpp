#include "settings.h"

#include <Preferences.h>

namespace {
Preferences prefs;
TulSettings cfg;
}

void settingsBegin() {
    prefs.begin("tul-reader", false);
    cfg.brightness = prefs.getUChar("bright", 200);
    cfg.screensaverSeconds = prefs.getUShort("screen", 300);
    cfg.wifiEnabled = prefs.getBool("wifi", true);
    cfg.bleEnabled = prefs.getBool("ble", false);
    cfg.autoConnect = prefs.getBool("auto", true);
}

TulSettings &settings() {
    return cfg;
}

void settingsSave() {
    prefs.putUChar("bright", cfg.brightness);
    prefs.putUShort("screen", cfg.screensaverSeconds);
    prefs.putBool("wifi", cfg.wifiEnabled);
    prefs.putBool("ble", cfg.bleEnabled);
    prefs.putBool("auto", cfg.autoConnect);
}
