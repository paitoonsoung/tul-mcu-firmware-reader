#include "storage_manager.h"

#include <SD.h>
#include <SPI.h>
#include "board_config.h"

namespace {
bool ready = false;
}

bool storageBegin() {
    SPI.begin(PIN_SD_SCK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);
    ready = SD.begin(PIN_SD_CS, SPI, 20000000);
    return ready;
}

bool storageFormat() {
    if (!ready && !storageBegin()) {
        return false;
    }
    return SD.format();
}

uint64_t storageCapacityBytes() {
    return ready ? SD.cardSize() : 0;
}

uint64_t storageFreeBytes() {
    return ready ? (SD.totalBytes() - SD.usedBytes()) : 0;
}

uint64_t storageUsedBytes() {
    return ready ? SD.usedBytes() : 0;
}
