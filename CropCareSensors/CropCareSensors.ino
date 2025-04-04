// Classess and Libraries
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
  lcdSetup();
  tempSensor.begin();  // Starts the Temperature Sensor
}
void loop() {
  lcdTemperatureSensor();
  lcdSoilMoistureSensor();
  phSensor.readPh();
  delay(1000);
}
void lcdSetup(){
  lcd.init();
  lcd.backlight();
  lcd.setCursor(4, 0);
  lcd.print("CropCare");
  lcd.setCursor(5, 1);
  lcd.print("System");
  delay(4000);
  lcd.clear();
}
void lcdSoilMoistureSensor(){
  int moisture = soilSensor.getMoisturePercent();     // read ONCE
  String condition;

  // Determine condition based on same percent
  if (moisture <= 10) condition = "Very Dry";
  else if (moisture <= 40) condition = "Dry";
  else if (moisture <= 80) condition = "Moist";
  else if (moisture < 100) condition = "Very Wet";
  else condition = "Submerged";

  // Serial output for checking
  Serial.print("Moisture: ");
  Serial.print(moisture);
  Serial.println(" %");

  Serial.print("Condition: ");
  Serial.println(condition);

  // LCD Output
  lcd.setCursor(0, 1);
  lcd.print("MP:");
  lcd.print(moisture);
  lcd.print("%");

  lcd.setCursor(7, 1);
  lcd.print("          ");  // clear old condition
  lcd.setCursor(7, 1);
  lcd.print(condition);
}

void lcdTemperatureSensor(){
  float tempC = tempSensor.getCelsius();     // Passed the celcius value in the float from TemperatureSensor.cpp
  float tempF = (tempC * 9.0 / 5.0) + 32.0;  //  Converts the celcius into fahrenheit manually
  // Prints the Temperature and Fahrenheit Output
  Serial.print("\nTemperature: ");
  Serial.print(tempC);
  Serial.println(" °C");
  Serial.print("Fahrenheit: ");
  Serial.println(tempF);

  lcd.setCursor(9, 0);
  lcd.print(tempC, 2);
  lcd.write(223); //This prints the  ° symbol
  lcd.print("C");
}