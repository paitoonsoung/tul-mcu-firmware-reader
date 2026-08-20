#include "storage_manager.h"

#include <SD.h>
#include <SPI.h>
#include "ff.h"
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

    // Arduino-ESP32 3.3.x SDFS has no public format() method.
    // Use the bundled FatFs formatter while the SD driver is initialized.
    const FRESULT mountResult = f_mount(nullptr, "0:", 0);
    if (mountResult != FR_OK) {
        return false;
    }

    BYTE work[FF_MAX_SS];
    const MKFS_PARM options = {
        static_cast<BYTE>(FM_ANY),
        0,
        0,
        0,
        0
    };

    const FRESULT formatResult = f_mkfs("0:", &options, work, sizeof(work));
    if (formatResult != FR_OK) {
        // Restore the normal Arduino SD mount before returning.
        SD.end();
        ready = storageBegin();
        return false;
    }

    // Recreate the Arduino VFS/FatFs mount after formatting.
    SD.end();
    ready = storageBegin();
    return ready;
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
