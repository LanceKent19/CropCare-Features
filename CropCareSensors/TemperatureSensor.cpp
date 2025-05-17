// Libraries for Temperature Sensor
#include "TemperatureSensor.h"
#include "config.h"
TemperatureSensor::TemperatureSensor(int pin, LiquidCrystal_I2C &lcdRef, WiFiManager &wifiManager)  // The constructor for the Temperature Sensor Class
  // Initializer List
  : pin(pin), oneWire(pin), sensors(&oneWire), lcd(lcdRef), wifiManager(wifiManager) {}

void TemperatureSensor::begin() {
  sensors.begin();  // setup to initialize the temperature sensor
}

void TemperatureSensor::requestTemperature() {
  sensors.requestTemperatures();  // non-blocking trigger
}

void TemperatureSensor::forcePowerOffUpdate() {
  SensorData data;
  data.powerState = "off";
  data.temperature = NAN;
  wifiManager.sendAllSensorData(serverURL, data);
}
void TemperatureSensor::lcdTemperatureSensor() {
  lastTemperature = sensors.getTempCByIndex(0);

  char buffer[16];
  sprintf(buffer, "%5.2f%cC", lastTemperature, 223);  // 5 characters wide, 2 decimals, °, and C
  lcd.setCursor(9, 0);                                // Position it to top row, right side
  lcd.print(buffer);
}
