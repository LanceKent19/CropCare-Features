#ifndef SOIL_MOISTURE_SENSOR_H
#define SOIL_MOISTURE_SENSOR_H

#include <Arduino.h>

class SoilMoistureSensor {  // the class of the headline file
private:                    // private variables for accessing internally. Abstraction
  int pin;
  int dryValue, wetValue;

public:
  SoilMoistureSensor(int pin, int dryVal, int wetVal);  // Creates a constructor with requesting the value of pins, the dry value and wet value
  int readValue();                                      // variable storing the read value of the soil moisture sensor
  int getMoisturePercent();                             // getter for returning the percent of the moisture
  String getCondition();                                // returning the condition of the land if it is dry, wet or moist
};

#endif
