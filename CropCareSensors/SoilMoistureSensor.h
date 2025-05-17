#ifndef SOIL_MOISTURE_SENSOR_H
#define SOIL_MOISTURE_SENSOR_H
  
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "WifiManager.h"

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
  WiFiManager &wifiManager;
  int lastMoisture = -1; // Add this to store last value
public:
  SoilMoistureSensor(int pin, int dryVal, int wetVal, int ledPin, int buzzerPin, LiquidCrystal_I2C &lcd, WiFiManager &wifiManager);
  int readValue();
  int getMoisturePercent();
  int getLastValue() const { return lastMoisture; } // Add this method

  void update();

  void forcePowerOffUpdate();
};

#endif
