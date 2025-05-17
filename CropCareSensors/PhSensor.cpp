#include "PhSensor.h"
#include "config.h"
PhSensor::PhSensor(int pin, float caliVal, int redLed, int greenLed, int blueLed, int buzzer,
                   LiquidCrystal_I2C& lcd, WiFiManager& wifiManager, PowerManager& powerManager)
  : pin(pin), caliVal(caliVal), redLed(redLed), greenLed(greenLed), blueLed(blueLed),
    buzzer(buzzer), lcd(lcd), wifiManager(wifiManager), powerManager(powerManager) {

  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  pinMode(buzzer, OUTPUT);

  if (!powerManager.isSystemOn()) {
    forcePowerOffUpdate();
  }
}

void PhSensor::readPhNonBlocking() {
  if (!powerManager.isSystemOn()) {
    lastPhValue = NAN;
    forcePowerOffUpdate();
    return;
  }

  static unsigned long lastReadTime = 0;
  if (millis() - lastReadTime < 1000) {
    return;  // Now 1 second interval (more stable)
  }
  lastReadTime = millis();

  const int sampleCount = 20;
  int buffer_arr[sampleCount];

  // Sample readings
  for (int i = 0; i < sampleCount; i++) {
    buffer_arr[i] = analogRead(pin);
    delayMicroseconds(300);  // Tiny delay to let ADC stabilize
  }

  // Sort the array for median filtering
  std::sort(buffer_arr, buffer_arr + sampleCount);

  // Calculate the average of middle values (ignore outliers)
  float avgval = 0;
  for (int i = 5; i < 15; i++) {  // Ignore lowest 5 and highest 5
    avgval += buffer_arr[i];
  }
  avgval /= 10;  // Average of 10 middle samples

  // Calculate voltage and pH
  float volt = (avgval * 3.3) / 4095.0;
  float ph_act = -5.88 * volt + caliVal;  // Updated calibration line

  // Save last valid value
  lastPhValue = ph_act;

  char buffer[16];
  snprintf(buffer, sizeof(buffer), "PH: %.2f", lastPhValue);
  lcd.setCursor(0, 0);
  lcd.print(buffer);

  updateIndicators(ph_act);
}

void PhSensor::updateIndicators(float ph_act) {
  static unsigned long buzzerStartTime = 0;
  static bool buzzerActive = false;

  if (ph_act <= 6.5) {
    digitalWrite(redLed, HIGH);
    digitalWrite(greenLed, LOW);
    digitalWrite(blueLed, LOW);
    // digitalWrite(buzzer, HIGH);
    buzzerStartTime = millis();
    buzzerActive = true;
    Serial.println("Acidic");
  } else if (ph_act <= 7.5) {
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, HIGH);
    digitalWrite(blueLed, LOW);
    // digitalWrite(buzzer, LOW);
    buzzerActive = false;
    Serial.println("Neutral");
  } else {
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, LOW);
    digitalWrite(blueLed, HIGH);
    // digitalWrite(buzzer, HIGH);
    buzzerStartTime = millis();
    buzzerActive = true;
    Serial.println("Alkaline");
  }

  // Handle buzzer auto-off
  if (buzzerActive && millis() - buzzerStartTime >= 200) {
    digitalWrite(buzzer, LOW);
    buzzerActive = false;
  }
}
void PhSensor::forcePowerOffUpdate() {
  SensorData data;
  data.powerState = "off";
  data.ph = NAN;
  wifiManager.sendAllSensorData(serverURL, data);
}