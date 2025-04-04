#ifndef PH_SENSOR_H
#define PH_SENSOR_H

// Libraries with the communication of the PH Level Sensors
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class PhSensor {
private:
  int pin;
  float caliVal;
  int buffer_arr[10];
  LiquidCrystal_I2C& lcd;  // Pass by reference (note the &)

public:
  PhSensor(int pin, float caliVal, LiquidCrystal_I2C& lcd); // Constructor also uses &
  void setupDisplay();
  float readPh();
};

#endif
