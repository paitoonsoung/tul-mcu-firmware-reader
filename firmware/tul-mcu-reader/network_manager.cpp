#include "network_manager.h"

#include <WiFi.h>
#include <Preferences.h>
#include <BLEDevice.h>
#include "settings.h"

namespace {
Preferences prefs;
String savedSsid;
String savedPassword;
String ipText = "0.0.0.0";
String ssidText = "NOT CONNECTED";
int rssiValue = 0;
}

void networkBegin() {
    prefs.begin("tul-wifi", true);
    savedSsid = prefs.getString("ssid", "");
    savedPassword = prefs.getString("pass", "");
    prefs.end();

    if (!settings().wifiEnabled) {
        WiFi.mode(WIFI_OFF);
        return;
    }

    WiFi.mode(WIFI_STA);
    networkConnectSaved();
}

void networkLoop() {
    if (WiFi.status() == WL_CONNECTED) {
        ssidText = WiFi.SSID();
        ipText = WiFi.localIP().toString();
        rssiValue = WiFi.RSSI();
    } else {
        ssidText = "NOT CONNECTED";
        ipText = "0.0.0.0";
        rssiValue = 0;
    }
}

void networkToggleWiFi() {
    settings().wifiEnabled = !settings().wifiEnabled;

    if (!settings().wifiEnabled) {
        WiFi.scanDelete();
        WiFi.disconnect(true, false);
        WiFi.mode(WIFI_OFF);
        ssidText = "WIFI OFF";
        ipText = "0.0.0.0";
        rssiValue = 0;
    } else {
        WiFi.mode(WIFI_STA);
        networkConnectSaved();
        ssidText = "CONNECTING...";
    }

    settingsSave();
}

void networkSaveCredentials(const String &ssid, const String &password) {
    savedSsid = ssid;
    savedPassword = password;

    prefs.begin("tul-wifi", false);
    prefs.putString("ssid", savedSsid);
    prefs.putString("pass", savedPassword);
    prefs.end();
}

void networkConnectSaved() {
    if (!settings().wifiEnabled || savedSsid.isEmpty()) return;

    WiFi.mode(WIFI_STA);
    WiFi.disconnect(false, false);
    WiFi.begin(savedSsid.c_str(), savedPassword.c_str());
}

void networkScanWiFi() {
    if (!settings().wifiEnabled) return;

    WiFi.mode(WIFI_STA);
    WiFi.scanDelete();
    Serial.println("WiFi scan started");
    const int16_t result = WiFi.scanNetworks(true, true);
    Serial.printf("WiFi scan request: %d\n", result);
}

void networkToggleBLE() {
    settings().bleEnabled = !settings().bleEnabled;

    if (settings().bleEnabled) {
        BLEDevice::init("TUL-MCU-Reader");
        Serial.println("BLE: ON");
    } else {
        BLEDevice::deinit(true);
        Serial.println("BLE: OFF");
    }

    settingsSave();
}

const char *networkSSID() { return ssidText.c_str(); }
const char *networkIP() { return ipText.c_str(); }
int networkRSSI() { return rssiValue; }
int networkScanCount() { return WiFi.scanComplete(); }
String networkScanName(int index) { return WiFi.SSID(index); }
int networkScanRSSI(int index) { return WiFi.RSSI(index); }
