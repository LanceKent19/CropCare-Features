// Libraries for Temperature Sensor
#include "TemperatureSensor.h"

TemperatureSensor::TemperatureSensor(int pin, LiquidCrystal_I2C &lcdRef)  // The constructor for the Temperature Sensor Class
  // Initializer List
  : pin(pin), oneWire(pin), sensors(&oneWire), lcd(lcdRef) {}

void TemperatureSensor::begin() {
  sensors.begin();  // setup to initialize the temperature sensor
}

float TemperatureSensor::getCelsius() {
  sensors.requestTemperatures();      // Ask the sensor for the temperature
  return sensors.getTempCByIndex(0);  // return the value in celcius
}

void TemperatureSensor::lcdTemperatureSensor() {
  float tempC = getCelsius();
  float tempF = (tempC * 9.0 / 5.0) + 32.0;

  Serial.print("\nTemperature: ");
  Serial.print(tempC);
  Serial.println(" °C");
  Serial.print("Fahrenheit: ");
  Serial.println(tempF);

  lcd.setCursor(9, 0);
  lcd.print(tempC, 2);
  lcd.write(223); // °
  lcd.print("C");
}
