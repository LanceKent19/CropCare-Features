#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class PowerManager {
private:
    bool systemPoweredOn;
    LiquidCrystal_I2C& lcd;
    
public:
    // Initialize in OFF state by default
    PowerManager(LiquidCrystal_I2C& lcdRef) : lcd(lcdRef), systemPoweredOn(false) {}
    
    void begin() {
        if (!systemPoweredOn) {
            lcd.noBacklight();
            lcd.clear();
            // lcd.print("SYSTEM OFF");
        }
    }
    
    void powerOnSystem() {
        systemPoweredOn = true;
        lcd.backlight();
        lcd.clear();
    }
    
    void powerOffSystem() {
        systemPoweredOn = false;
        lcd.clear();
        lcd.print("SYSTEM OFF");
        lcd.noBacklight();
    }
    
    bool isSystemOn() const {
        return systemPoweredOn;
    }
};

#endif