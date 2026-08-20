#pragma once

#include <Arduino.h>

// WiFi.scanComplete() returns -1 while an async scan is running.
constexpr int NETWORK_SCAN_RUNNING = -1;

void networkBegin();
void networkLoop();
void networkToggleWiFi();
void networkScanWiFi();
void networkToggleBLE();
void networkSaveCredentials(const String &ssid, const String &password);
void networkConnectSaved();
const char *networkSSID();
const char *networkIP();
int networkRSSI();
int networkScanCount();
String networkScanName(int index);
int networkScanRSSI(int index);
