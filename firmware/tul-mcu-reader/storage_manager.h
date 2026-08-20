#pragma once

#include <Arduino.h>

bool storageBegin();
bool storageFormat();
uint64_t storageCapacityBytes();
uint64_t storageFreeBytes();
uint64_t storageUsedBytes();
