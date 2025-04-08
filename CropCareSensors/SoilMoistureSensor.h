#ifndef SOIL_MOISTURE_SENSOR_H
#define SOIL_MOISTURE_SENSOR_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class SoilMoistureSensor {
private:
  int pin;
  int dryValue, wetValue;
  int ledPin, buzzerPin;
  LiquidCrystal_I2C &lcd;
  unsigned long dryStartTime;
  bool dryTimerStarted;
  const unsigned long dryDelay = 3000;

  bool ledState;

public:
  SoilMoistureSensor(int pin, int dryVal, int wetVal, int ledPin, int buzzerPin, LiquidCrystal_I2C &lcd);
  int readValue();
  int getMoisturePercent();
  void update(bool isActive);
  void powerOn();
  void powerOff();
};

#endif
