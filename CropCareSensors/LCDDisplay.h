#ifndef LCDDISPLAY_H
#define LCDDISPLAY_H

#include <LiquidCrystal_I2C.h>

class LCDDisplay {
private:
  LiquidCrystal_I2C lcd;

public:
  LCDDisplay()
    : lcd(0x27, 16, 2) {}

  void begin() {
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("CropCare System");
    lcd.setCursor(0, 1);
    lcd.print("Setup...");
    delay(2000);
    lcd.clear();
    lcd.noBacklight();
  }
  void showPowerStatus(bool isOn) {
    lcd.clear();
    lcd.setCursor(3, 0);
    if (isOn) {
      lcd.backlight();
      lcd.print("POWER ON");
      delay(1000);
    } else {
      lcd.print("POWER OFF");
      delay(1000);
      lcd.noBacklight();
    }
    lcd.clear();
  }

  LiquidCrystal_I2C& getLCD() {
    return lcd;
  }
};

#endif