#pragma once

#include <Arduino.h>

void networkBegin();
void networkLoop();
void networkToggleWiFi();
void networkScanWiFi();
void networkToggleBLE();
const char *networkSSID();
const char *networkIP();
int networkRSSI();
