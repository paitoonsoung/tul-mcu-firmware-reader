#include <Arduino.h>
#include "board_config.h"
#include "atmega328p_isp.h"

Atmega328pIsp isp;

void setup() {
    Serial.begin(115200);
    delay(300);

    Serial.println();
    Serial.println("TUL MCU Firmware Reader");
    Serial.println("ATmega328P ISP Proof of Concept");
    Serial.println("No erase/write operations");

    isp.begin();

    uint8_t signature[3] = {0};
    if (!isp.readSignature(signature)) {
        Serial.println("ISP communication failed");
        return;
    }

    Serial.printf("ATmega328P signature: %02X %02X %02X\n",
                  signature[0], signature[1], signature[2]);

    if (signature[0] == 0x1E &&
        signature[1] == 0x95 &&
        signature[2] == 0x0F) {
        Serial.println("TARGET OK");
    } else {
        Serial.println("UNEXPECTED SIGNATURE");
    }
}

void loop() {
    delay(1000);
}
