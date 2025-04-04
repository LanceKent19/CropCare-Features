// Libraries for Temperature Sensor
#include "TemperatureSensor.h"

TemperatureSensor::TemperatureSensor(int pin)  // The constructor for the Temperature Sensor Class
  // Initializer List
  : pin(pin), oneWire(pin), sensors(&oneWire) {}

void TemperatureSensor::begin() {
  sensors.begin();  // setup to initialize the temperature sensor
}

float TemperatureSensor::getCelsius() {
  sensors.requestTemperatures();      // Ask the sensor for the temperature
  return sensors.getTempCByIndex(0);  // return the value in celcius
}

