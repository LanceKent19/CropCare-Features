#ifndef PH_SENSOR_H
#define PH_SENSOR_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "WifiManager.h"
#include "PowerManager.h"

class PhSensor {
private:
  // Hardware pins and calibration
  int pin;
  float caliVal;

  // Indicator pins
  int redLed;
  int greenLed;
  int blueLed;
  int buzzer;

  // Reading state
  enum class ReadingState { IDLE, SAMPLING, PROCESSING };
  ReadingState readingState = ReadingState::IDLE;
  unsigned long lastSampleTime = 0;
  int sampleIndex = 0;
  int buffer_arr[10];
  float lastPhValue = NAN;

  // Buzzer control
  unsigned long buzzerStartTime = 0;
  bool buzzerActive = false;

  // Dependencies
  LiquidCrystal_I2C& lcd;
  WiFiManager& wifiManager;
  PowerManager& powerManager;

public:
  PhSensor(int pin, float caliVal, int redLed, int greenLed, int blueLed, int buzzer,
           LiquidCrystal_I2C& lcd, WiFiManager& wifiManager, PowerManager& powerManager);
            
  void startNewReading();
  float getLastValue() const { return lastPhValue; } // Add this method
  void readPhNonBlocking(bool showOnLCD);
  void forcePowerOffUpdate();
  void updateIndicators(float ph_act);
};

#endif