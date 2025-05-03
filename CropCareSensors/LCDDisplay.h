#ifndef LCDDISPLAY_H
#define LCDDISPLAY_H

#include <LiquidCrystal_I2C.h>

class LCDDisplay {
private:
   LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);
  bool isSystemOn = false;
  
public:
  LCDDisplay() : lcd(0x27, 16, 2) {}

  void begin() {
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("CropCare System");
    lcd.setCursor(0, 1);
    lcd.print("Setup...");
    delay(2000);
    lcd.clear();
  }
    LiquidCrystal_I2C* getLCDPointer() {
    return &lcd;
}
  void showPowerStatus(bool isOn) {
    isSystemOn = isOn;
    lcd.clear();
    if (isOn) {
      lcd.backlight();
      lcd.setCursor(3, 0);
      lcd.print("POWER ON");
      lcd.setCursor(2, 1);
      lcd.print("Initializing...");
    } else {
      lcd.backlight(); // Temporarily enable for message
      lcd.setCursor(3, 0);
      lcd.print("POWER OFF");
      lcd.setCursor(0, 1);
      lcd.print("System sleeping");
      delay(1000);
      lcd.noBacklight();
    }
    delay(1000);
    lcd.clear();
  }
  void printTemporaryMessage(String message, unsigned long durationMs = 1500) {
    lcd.clear();
    lcd.setCursor((16 - message.length()) / 2, 0);
    lcd.print(message);
    delay(durationMs);
    lcd.clear();
} 
void printCenter(String message) {
    lcd.clear();
    lcd.setCursor((16 - message.length()) / 2, 0); // center horizontally
    lcd.print(message);
}

  void showNoData() {
    if (!isSystemOn) return;
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("No sensor data");
    lcd.setCursor(0, 1);
    lcd.print("available");
  }

  LiquidCrystal_I2C& getLCD() {
    return lcd;
  }
  
  bool isOn() const {
    return isSystemOn;
  }

};
#endif