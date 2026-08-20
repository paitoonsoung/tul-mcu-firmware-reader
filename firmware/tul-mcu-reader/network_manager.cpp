#include "network_manager.h"

#include <WiFi.h>
#include <Preferences.h>
#include <BLEDevice.h>
#include "settings.h"

namespace {
Preferences prefs;
String savedSsid;
String savedPassword;
String ipText;
String ssidText;
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
        WiFi.disconnect(true, false);
        WiFi.mode(WIFI_OFF);
    } else {
        WiFi.mode(WIFI_STA);
        networkConnectSaved();
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
    WiFi.begin(savedSsid.c_str(), savedPassword.c_str());
}

void networkScanWiFi() {
    if (!settings().wifiEnabled) return;
    WiFi.mode(WIFI_STA);
    WiFi.scanDelete();
    // Async scan prevents the UI task from blocking during RF scanning.
    WiFi.scanNetworks(true, true);
}

void networkToggleBLE() {
    settings().bleEnabled = !settings().bleEnabled;
    if (settings().bleEnabled) BLEDevice::init("TUL-MCU-Reader");
    else BLEDevice::deinit(true);
    settingsSave();
}

const char *networkSSID() { return ssidText.c_str(); }
const char *networkIP() { return ipText.c_str(); }
int networkRSSI() { return rssiValue; }
int networkScanCount() { return WiFi.scanComplete(); }
String networkScanName(int index) { return WiFi.SSID(index); }
int networkScanRSSI(int index) { return WiFi.RSSI(index); }
