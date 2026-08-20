#include "atmega328p_isp.h"
#include "board_config.h"

#include <SD.h>
#include <SPI.h>

namespace {
constexpr uint8_t CMD_PROGRAM_ENABLE = 0xAC;
constexpr uint8_t CMD_PROGRAM_ENABLE_MAGIC = 0x53;
constexpr uint8_t CMD_READ_SIGNATURE = 0x30;
constexpr uint8_t CMD_LOAD_ADDRESS_LOW = 0x20;
constexpr uint8_t CMD_LOAD_ADDRESS_HIGH = 0x28;
constexpr uint8_t EXPECTED_PROGRAM_ENABLE = 0x53;
constexpr uint32_t FLASH_BYTES = 32768UL;
}

void Atmega328pIsp::begin() {
    pinMode(PIN_ISP_MOSI, OUTPUT);
    pinMode(PIN_ISP_SCK, OUTPUT);
    pinMode(PIN_TARGET_RESET, OUTPUT);
    pinMode(PIN_ISP_MISO, INPUT);

    digitalWrite(PIN_ISP_MOSI, LOW);
    digitalWrite(PIN_ISP_SCK, LOW);
    digitalWrite(PIN_TARGET_RESET, HIGH);
}

void Atmega328pIsp::writeBit(uint8_t bit) {
    digitalWrite(PIN_ISP_MOSI, bit ? HIGH : LOW);
    delayMicroseconds(ISP_HALF_PERIOD_US);
    digitalWrite(PIN_ISP_SCK, HIGH);
    delayMicroseconds(ISP_HALF_PERIOD_US);
}

uint8_t Atmega328pIsp::readBit() {
    digitalWrite(PIN_ISP_SCK, HIGH);
    delayMicroseconds(ISP_HALF_PERIOD_US);
    const uint8_t bit = digitalRead(PIN_ISP_MISO) ? 1 : 0;
    digitalWrite(PIN_ISP_SCK, LOW);
    delayMicroseconds(ISP_HALF_PERIOD_US);
    return bit;
}

uint8_t Atmega328pIsp::transfer(uint8_t value) {
    uint8_t received = 0;

    for (uint8_t mask = 0x80; mask != 0; mask >>= 1) {
        digitalWrite(PIN_ISP_MOSI, (value & mask) ? HIGH : LOW);
        delayMicroseconds(ISP_HALF_PERIOD_US);
        digitalWrite(PIN_ISP_SCK, HIGH);
        delayMicroseconds(ISP_HALF_PERIOD_US);
        if (digitalRead(PIN_ISP_MISO)) received |= mask;
        digitalWrite(PIN_ISP_SCK, LOW);
        delayMicroseconds(ISP_HALF_PERIOD_US);
    }
    return received;
}

bool Atmega328pIsp::enterProgrammingMode() {
    digitalWrite(PIN_ISP_SCK, LOW);
    digitalWrite(PIN_ISP_MOSI, LOW);
    digitalWrite(PIN_TARGET_RESET, HIGH);
    delay(5);
    digitalWrite(PIN_TARGET_RESET, LOW);
    delay(25);

    transfer(CMD_PROGRAM_ENABLE);
    transfer(CMD_PROGRAM_ENABLE_MAGIC);
    transfer(0x00);
    const uint8_t response = transfer(0x00);
    return response == EXPECTED_PROGRAM_ENABLE;
}

void Atmega328pIsp::leaveProgrammingMode() {
    digitalWrite(PIN_TARGET_RESET, HIGH);
    digitalWrite(PIN_ISP_SCK, LOW);
    digitalWrite(PIN_ISP_MOSI, LOW);
}

bool Atmega328pIsp::readSignature(uint8_t signature[3]) {
    if (signature == nullptr || !enterProgrammingMode()) {
        leaveProgrammingMode();
        return false;
    }

    for (uint8_t index = 0; index < 3; ++index) {
        transfer(CMD_READ_SIGNATURE);
        transfer(0x00);
        transfer(index);
        signature[index] = transfer(0x00);
    }

    leaveProgrammingMode();
    return true;
}

uint8_t Atmega328pIsp::readFlashByte(uint16_t wordAddress, bool highByte) {
    // AVR SPI read program memory: 0x20=low byte, 0x28=high byte.
    transfer(highByte ? CMD_LOAD_ADDRESS_HIGH : CMD_LOAD_ADDRESS_LOW);
    transfer(static_cast<uint8_t>(wordAddress >> 8));
    transfer(static_cast<uint8_t>(wordAddress & 0xFF));
    return transfer(0x00);
}

bool Atmega328pIsp::readFlashToSd(const char *path) {
    if (path == nullptr || !enterProgrammingMode()) {
        leaveProgrammingMode();
        return false;
    }

    if (!SD.begin(PIN_SD_CS, SPI, 20000000)) {
        leaveProgrammingMode();
        return false;
    }

    SD.remove(path);
    File file = SD.open(path, FILE_WRITE);
    if (!file) {
        leaveProgrammingMode();
        return false;
    }

    uint8_t buffer[256];
    uint32_t offset = 0;
    uint32_t wordAddress = 0;

    while (offset < FLASH_BYTES) {
        const uint16_t chunk = static_cast<uint16_t>(min<uint32_t>(sizeof(buffer), FLASH_BYTES - offset));
        for (uint16_t i = 0; i < chunk; i += 2) {
            const uint16_t address = static_cast<uint16_t>(wordAddress);
            buffer[i] = readFlashByte(address, false);
            if (i + 1 < chunk) buffer[i + 1] = readFlashByte(address, true);
            ++wordAddress;
        }

        if (file.write(buffer, chunk) != chunk) {
            file.close();
            leaveProgrammingMode();
            return false;
        }
        offset += chunk;
    }

    file.flush();
    file.close();
    leaveProgrammingMode();
    return true;
}
