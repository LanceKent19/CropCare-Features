#include "TemperatureSensor.h"
#include "SoilMoistureSensor.h"
#include "PhSensor.h"
#include <LiquidCrystal_I2C.h>

// Define objects
TemperatureSensor tempSensor(23);               // Creates and object of the class TemperatureSensor and passed the 23 GPIO pin
SoilMoistureSensor soilSensor(34, 3500, 1000);  // Creates and object of the class SoilMoistureSensor and passed the 34 GPIO pin with dry and wet value
LiquidCrystal_I2C lcd(0x27, 16, 2);             // Creates and object of the lcd that is 16x2
PhSensor phSensor(35, 21.34, lcd);              // Creates and object of the class PhSensor and passed the 35 GPIO pin

void setup() {
  Serial.begin(115200);
  tempSensor.begin();  // Starts the Temperature Sensor
  phSensor.setupDisplay();
}

void loop() {
  float tempC = tempSensor.getCelsius();     // Passed the celcius value in the float from TemperatureSensor.cpp
  float tempF = (tempC * 9.0 / 5.0) + 32.0;  //  Converts the celcius into fahrenheit manually
  // Prints the Temperature and Fahrenheit Output
  Serial.print("\nTemperature: ");
  Serial.print(tempC);
  Serial.println(" °C");
  Serial.print("Fahrenheit: ");
  Serial.println(tempF);

  lcd.setCursor(10, 0);
  lcd.print(tempC, 2);

  // Prints the Soil Moisture Value with the condition
  Serial.print("Moisture: ");
  Serial.print(soilSensor.getMoisturePercent());
  Serial.println(" %");
  Serial.print("Condition: ");
  Serial.println(soilSensor.getCondition());

  lcd.setCursor(0, 1);
  lcd.print("SM: ");
  lcd.setCursor(4, 1);
  lcd.print(soilSensor.getCondition());

  phSensor.readPh();

  delay(1000);
}
