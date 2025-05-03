#include "HumiditySensor.h"
#include "config.h"
HumiditySensor::HumiditySensor(int humidityPin, LiquidCrystal_I2C& display, WiFiManager& wifiManager)
  : pin(humidityPin), dht(humidityPin), lcd(&display), wifiManager(wifiManager) {}

void HumiditySensor::begin() {
  dht.begin();
}

void HumiditySensor::update(bool showOnLCD) {
  if (dht.getData()) {
      lastHumidity = dht.getHumidity(); // Store the value
    Serial.print("Humidity: ");
    Serial.print(lastHumidity);
    Serial.println(" %");

  if (showOnLCD) {
    char buffer[16];
    sprintf(buffer, "Hum:%3d%%", lastHumidity);  // right-aligned in 3 spaces
    lcd->setCursor(8, 1);
    lcd->print(buffer);
  }
  } else {
    lastHumidity = NAN; // Mark as invalid on failure
    Serial.println("Failed to read humidity");
  }
}

void HumiditySensor::forcePowerOffUpdate() {
  SensorData data;
  data.powerState = "off";
  data.humidity = NAN;
  wifiManager.sendAllSensorData(serverURL, data);
}
