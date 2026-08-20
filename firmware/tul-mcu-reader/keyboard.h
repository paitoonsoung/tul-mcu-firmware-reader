#pragma once

#include <Arduino.h>
#include <LovyanGFX.hpp>

namespace tului {

template <typename Display>
String keyboard(Display &d, const char *title, bool passwordMode = false) {
    String value;
    bool shift = true;
    const char *lower = "qwertyuiopasdfghjklzxcvbnm";
    const char *upper = "QWERTYUIOPASDFGHJKLZXCVBNM";

    while (true) {
        d.fillScreen(TFT_BLACK);
        d.setTextColor(TFT_CYAN); d.setTextSize(2); d.setCursor(12, 8); d.print(title);
        d.setTextColor(TFT_WHITE); d.setCursor(12, 38);
        if (passwordMode) {
            for (size_t i = 0; i < value.length(); ++i) d.print('*');
        } else d.print(value);

        for (int i = 0; i < 26; ++i) {
            const int row = i / 10;
            const int col = i % 10;
            const int x = 8 + col * 46;
            const int y = 70 + row * 38;
            d.drawRect(x, y, 40, 32, TFT_CYAN);
            d.setCursor(x + 13, y + 8); d.print(shift ? upper[i] : lower[i]);
        }
        const int y = 188;
        d.drawRect(8, y, 40, 32, TFT_CYAN); d.setCursor(19, y + 8); d.print("1");
        d.drawRect(54, y, 40, 32, TFT_CYAN); d.setCursor(65, y + 8); d.print("2");
        d.drawRect(100, y, 40, 32, TFT_CYAN); d.setCursor(111, y + 8); d.print("3");
        d.drawRect(146, y, 40, 32, TFT_CYAN); d.setCursor(157, y + 8); d.print("4");
        d.drawRect(192, y, 40, 32, TFT_CYAN); d.setCursor(203, y + 8); d.print("5");
        d.drawRect(238, y, 40, 32, TFT_CYAN); d.setCursor(249, y + 8); d.print("6");
        d.drawRect(284, y, 40, 32, TFT_CYAN); d.setCursor(295, y + 8); d.print("7");
        d.drawRect(330, y, 40, 32, TFT_CYAN); d.setCursor(341, y + 8); d.print("8");
        d.drawRect(376, y, 40, 32, TFT_CYAN); d.setCursor(387, y + 8); d.print("9");
        d.drawRect(422, y, 40, 32, TFT_CYAN); d.setCursor(433, y + 8); d.print("0");

        d.drawRect(8, 230, 90, 40, TFT_YELLOW); d.setCursor(20, 242); d.print("SHIFT");
        d.drawRect(108, 230, 170, 40, TFT_CYAN); d.setCursor(170, 242); d.print("SPACE");
        d.drawRect(288, 230, 90, 40, TFT_CYAN); d.setCursor(305, 242); d.print("BKSP");
        d.drawRect(388, 230, 80, 40, TFT_GREEN); d.setCursor(402, 242); d.print("ENTER");

        uint16_t tx, ty;
        if (!d.getTouch(&tx, &ty)) { delay(20); continue; }
        if (ty >= 70 && ty < 184) {
            const int row = (ty - 70) / 38;
            const int col = tx / 46;
            const int index = row * 10 + col;
            if (index >= 0 && index < 26 && col < 10) value += (shift ? upper[index] : lower[index]);
        } else if (ty >= 188 && ty < 222 && tx < 460) {
            value += char('1' + (tx / 46));
            if (tx >= 414) value.setCharAt(value.length() - 1, '0');
        } else if (ty >= 230 && ty < 275) {
            if (tx < 100) shift = !shift;
            else if (tx < 288) value += ' ';
            else if (tx < 388 && value.length()) value.remove(value.length() - 1);
            else { while (d.getTouch(&tx, &ty)) delay(10); return value; }
        }
        while (d.getTouch(&tx, &ty)) delay(10);
    }
}

} // namespace tului
