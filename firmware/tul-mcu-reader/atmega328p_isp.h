#pragma once

#include <Arduino.h>

class Atmega328pIsp {
public:
    void begin();
    bool readSignature(uint8_t signature[3]);
    bool readFlashToSd(const char *path = "/ATmega328P_flash.bin");

private:
    uint8_t transfer(uint8_t value);
    bool enterProgrammingMode();
    void leaveProgrammingMode();
    void writeBit(uint8_t bit);
    uint8_t readBit();
    uint8_t readFlashByte(uint16_t wordAddress, bool highByte);
};
