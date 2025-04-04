#include "SoilMoistureSensor.h"  // includes the code

SoilMoistureSensor::SoilMoistureSensor(int pin, int dryVal, int wetVal)  // the constructor
  // Initialize List from the constructor
  : pin(pin), dryValue(dryVal), wetValue(wetVal) {}

int SoilMoistureSensor::readValue() {  // method for returning the pin and reading values
  return analogRead(pin);
}

int SoilMoistureSensor::getMoisturePercent() {                             // method for retunning the percent of the moisture
  int value = readValue();                                                 // store the readValue into value
  int percent = 100 - ((value - wetValue) * 100 / (dryValue - wetValue));  // computation into percentation of the soil
  return constrain(percent, 0, 100);
}
