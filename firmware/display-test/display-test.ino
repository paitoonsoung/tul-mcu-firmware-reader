#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// WT32-S3-WROVER / PanLee display wiring used by the previous TUL display build.
constexpr uint8_t TFT_CS = 1;
constexpr uint8_t TFT_RST = 2;
constexpr uint8_t TFT_DC = 42;
constexpr uint8_t TFT_BL = 3;
constexpr uint8_t TFT_MOSI = 13;
constexpr uint8_t TFT_SCLK = 12;

Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);

void setup() {
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
    tft.begin();
    tft.setRotation(1);
    tft.fillScreen(ILI9341_BLACK);

    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(3);
    tft.setCursor(52, 55);
    tft.print("TUL");

    tft.setTextSize(2);
    tft.setCursor(28, 105);
    tft.print("DIGITAL AUDIO");

    tft.setTextSize(2);
    tft.setCursor(62, 145);
    tft.print("MCU READER");

    tft.drawRect(20, 35, 280, 145, ILI9341_WHITE);

    tft.setTextSize(1);
    tft.setCursor(82, 205);
    tft.print("ESP32-S3 / WT32-S3");

    Serial.begin(115200);
    Serial.println();
    Serial.println("TUL MCU Reader - Display Test");
    Serial.println("ILI9341 display initialized");
}

void loop() {
    delay(1000);
}
