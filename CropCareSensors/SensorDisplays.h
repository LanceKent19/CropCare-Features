#ifndef SENSOR_DISPLAY_H
#define SENSOR_DISPLAY_H

#include <LiquidCrystal_I2C.h>
#include "SoilMoistureSensor.h"

class MoistureDisplay {
private:
    LiquidCrystal_I2C* lcd;
    SoilMoistureSensor* sensor;
    bool isOn = false;

public:
    MoistureDisplay(LiquidCrystal_I2C* lcdRef, SoilMoistureSensor* sensorRef) {
        lcd = lcdRef;
        sensor = sensorRef;
    }

    void setup() {
        lcd->init();
        lcd->backlight();
        lcd->setCursor(4, 0);
        lcd->print("CropCare");
        lcd->setCursor(5, 1);
        lcd->print("System");
        delay(4000);
        lcd->clear();
    }

    void showStartup() {
        lcd->clear();
        lcd->backlight();
        lcd->setCursor(3, 0);
        lcd->print("System On");
        delay(1000);
        lcd->clear();
    }

    void showShutdown() {
        lcd->clear();
        lcd->setCursor(3, 0);
        lcd->print("System OFF");
        delay(1000);
        lcd->clear();
        lcd->noBacklight();
    }

    void update() {
        int moisture = sensor->getMoisturePercent();
        String condition;

        if (moisture <= 10) condition = "Very Dry";
        else if (moisture <= 40) condition = "Dry";
        else if (moisture <= 80) condition = "Moist";
        else if (moisture < 100) condition = "Very Wet";
        else condition = "Submerged";

        Serial.print("Moisture: ");
        Serial.print(moisture);
        Serial.println(" %");

        Serial.print("Condition: ");
        Serial.println(condition);

        lcd->setCursor(0, 1);
        lcd->print("MP:");
        lcd->print(moisture);
        lcd->print("%");

        lcd->setCursor(7, 1);
        lcd->print("          ");
        lcd->setCursor(7, 1);
        lcd->print(condition);
    }
};

#endif
