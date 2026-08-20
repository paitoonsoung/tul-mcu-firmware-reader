#pragma once
#include <Arduino.h>
#include <LovyanGFX.hpp>
#include "network_manager.h"
#include "settings.h"
#include "storage_manager.h"
#include "atmega328p_isp.h"
#include "board_config.h"

namespace tului {

// WiFi.scanComplete() returns -1 while an async scan is running.
constexpr int WIFI_SCAN_RUNNING = NETWORK_SCAN_RUNNING;
