#ifndef LCDDISPLAY_H
#define LCDDISPLAY_H

#include <LiquidCrystal_I2C.h>

class LCDDisplay {
private:
  LiquidCrystal_I2C lcd;
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
    lcd.noBacklight();
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
  
  void displaySensorData(float ph, float temp, float moisture, float humidity) {
    if (!isSystemOn) return;
    
    lcd.clear();
    // Line 1: PH and Temperature
    lcd.setCursor(0, 0);
    lcd.print("PH:");
    lcd.print(ph, 1);
    lcd.setCursor(9, 0);
    lcd.print("T:");
    lcd.print(temp, 1);
    lcd.print((char)223); // Degree symbol
    lcd.print("C");
    
    // Line 2: Moisture and Humidity
    lcd.setCursor(0, 1);
    lcd.print("M:");
    lcd.print(moisture, 0);
    lcd.print("%");
    lcd.setCursor(9, 1);
    lcd.print("H:");
    lcd.print(humidity, 0);
    lcd.print("%");
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