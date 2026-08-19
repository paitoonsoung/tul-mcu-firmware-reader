#pragma once

#include <Arduino.h>

class Atmega328pIsp {
public:
    void begin();
    bool readSignature(uint8_t signature[3]);

private:
    uint8_t transfer(uint8_t value);
    bool enterProgrammingMode();
    void leaveProgrammingMode();
    void writeBit(uint8_t bit);
    uint8_t readBit();
};
