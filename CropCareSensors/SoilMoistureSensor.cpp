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

String SoilMoistureSensor::getCondition() {            // The method for identifying the condition of the land
  int percent = getMoisturePercent();                  // convert into int
  if (percent <= 10) return "Very Dry";                // Very dry condition
  else if (percent <= 40) return "Dry Soil";           // Dry condition
  else if (percent <= 80) return "Moist Soil";         // Moist Soil Condition
  else if (percent < 100) return "Very Wet/Near Mud";  // Very Wet or Near Mud Soil condition
  else return "Submerged in Water";                    // submerged in water if it reached the 100%
}
