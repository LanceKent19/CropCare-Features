#ifndef PH_SENSOR_H
#define PH_SENSOR_H

// Libraries with the communication of the PH Level Sensors
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class PhSensor {
private:  // private variables for abstraction
  int pin;
  float caliVal;
  int buffer_arr[10];
  LiquidCrystal_I2C lcd;  // objects for using the liquidcrystal

public:  // public constructor with requesting values of pin, calibrate value, and lcd
  PhSensor(int pin, float caliVal, LiquidCrystal_I2C lcd);
  void setupDisplay();  // method for initialize setup of PH Sensor
  float readPh();       // method for reading the ph level
};

#endif
