#pragma once

#include <Arduino.h>

// Initializes SPI SD interface and displays board/storage diagnostics.
// Returns true when the microSD card passes initialization and file I/O test.
bool runStorageAndMemoryTest();
