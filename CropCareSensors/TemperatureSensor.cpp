// Libraries for Temperature Sensor
#include "TemperatureSensor.h"

TemperatureSensor::TemperatureSensor(int pin, LiquidCrystal_I2C &lcdRef, WiFiManager &wifiManager)  // The constructor for the Temperature Sensor Class
  // Initializer List
  : pin(pin), oneWire(pin), sensors(&oneWire), lcd(lcdRef), wifiManager(wifiManager) {}

void TemperatureSensor::begin() {
  sensors.begin();  // setup to initialize the temperature sensor
}

// float TemperatureSensor::getCelsius() {
//   sensors.requestTemperatures();      // Ask the sensor for the temperature
//   return sensors.getTempCByIndex(0);  // return the value in celcius
// }

void TemperatureSensor::requestTemperature() {
  sensors.requestTemperatures();  // non-blocking trigger
}

void TemperatureSensor::sendTemperatureToServer(float temperature) {
  String body = "tempSensor=" + String(temperature) + "&powerState=on";
  wifiManager.sendHTTPPost(serverURL, body);
}

void TemperatureSensor::forcePowerOffUpdate() {
  String body = "tempSensor=-&powerState=off";
  wifiManager.sendHTTPPost(serverURL, body);
}
void TemperatureSensor::lcdTemperatureSensor(bool showOnLCD) {
  float tempC = sensors.getTempCByIndex(0);
  // float tempF = (tempC * 9.0 / 5.0) + 32.0;

  Serial.print("\nTemperature: ");
  Serial.print(tempC);
  Serial.println(" °C");
  // Serial.print("Fahrenheit: ");
  // Serial.println(tempF);
  if (showOnLCD) {
    char buffer[16];
    sprintf(buffer, "%5.2f%cC", tempC, 223);  // 5 characters wide, 2 decimals, °, and C
    lcd.setCursor(9, 0);                      // Position it to top row, right side
    lcd.print(buffer);
  }
  sendTemperatureToServer(tempC);
}
