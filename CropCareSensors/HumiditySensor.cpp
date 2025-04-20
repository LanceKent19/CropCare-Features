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

  if (showOnLCD) {
    char buffer[16];
    sprintf(buffer, "Hum:%3d%%", humidity);  // right-aligned in 3 spaces
    lcd->setCursor(8, 1);
    lcd->print(buffer);
  }

    sendHumidityToServer(humidity);
  }
}

void HumiditySensor::sendHumidityToServer(int humidity) {
  String body = "humiditySensor=" + String(humidity) + "&powerState=on";
  wifiManager.sendHTTPPost(serverURL, body);
}

void HumiditySensor::forcePowerOffUpdate() {
  String body = "humiditySensor=-&powerState=off";
  wifiManager.sendHTTPPost(serverURL, body);
}
