#include "HumiditySensor.h"

HumiditySensor::HumiditySensor(int humidityPin, LiquidCrystal_I2C& display, WiFiManager& wifiManager)
  : pin(humidityPin), dht(humidityPin), lcd(&display), wifiManager(wifiManager) {}

void HumiditySensor::begin() {
  dht.begin();
}

void HumiditySensor::update(bool showOnLCD) {
  if (dht.getData()) {
    int humidity = dht.getHumidity();
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    lcd->setCursor(8, 1);
    lcd->print("Hum:");
    lcd->setCursor(12, 1);
    lcd->print(humidity);
    lcd->print(" %");

    sendHumidityToServer(humidity);
  }
}