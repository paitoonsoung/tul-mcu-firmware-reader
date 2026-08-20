#include "storage_test.h"

#include <SD.h>
#include <SPI.h>

#include "board_config.h"

namespace {
constexpr size_t TEST_BUFFER_SIZE = 256;
const char *TEST_FILE = "/TUL_TEST.TXT";
}

bool runStorageAndMemoryTest() {
    Serial.println();
    Serial.println("=== TUL SYSTEM MEMORY / STORAGE TEST ===");

    Serial.printf("Internal RAM total : %lu bytes\n", ESP.getHeapSize());
    Serial.printf("Internal RAM free  : %lu bytes\n", ESP.getFreeHeap());
    Serial.printf("PSRAM total        : %lu bytes\n", ESP.getPsramSize());
    Serial.printf("PSRAM free         : %lu bytes\n", ESP.getFreePsram());
    Serial.printf("Flash chip size    : %lu bytes\n", ESP.getFlashChipSize());

    SPI.begin(PIN_SD_SCK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);

    if (!SD.begin(PIN_SD_CS, SPI, 20000000)) {
        Serial.println("microSD: INIT FAILED");
        return false;
    }

    const uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
        Serial.println("microSD: NO CARD");
        return false;
    }

    Serial.printf("microSD type        : %u\n", cardType);
    Serial.printf("microSD capacity    : %llu MB\n",
                  SD.cardSize() / (1024ULL * 1024ULL));
    Serial.printf("microSD filesystem  : %llu MB\n",
                  SD.totalBytes() / (1024ULL * 1024ULL));
    Serial.printf("microSD used        : %llu MB\n",
                  SD.usedBytes() / (1024ULL * 1024ULL));

    uint8_t buffer[TEST_BUFFER_SIZE];
    memset(buffer, 0xA5, sizeof(buffer));

    File file = SD.open(TEST_FILE, FILE_WRITE);
    if (!file) {
        Serial.println("microSD write test : FAILED (open)");
        return false;
    }

    const size_t written = file.write(buffer, sizeof(buffer));
    file.close();

    if (written != sizeof(buffer)) {
        SD.remove(TEST_FILE);
        Serial.println("microSD write test : FAILED (write)");
        return false;
    }

    file = SD.open(TEST_FILE, FILE_READ);
    if (!file) {
        SD.remove(TEST_FILE);
        Serial.println("microSD read test  : FAILED (open)");
        return false;
    }

    uint8_t verify[TEST_BUFFER_SIZE];
    const size_t read = file.read(verify, sizeof(verify));
    file.close();
    SD.remove(TEST_FILE);

    if (read != sizeof(verify) || memcmp(buffer, verify, sizeof(buffer)) != 0) {
        Serial.println("microSD read test  : FAILED (verify)");
        return false;
    }

    Serial.println("microSD read/write  : PASS");
    Serial.println("========================================");
    return true;
}
