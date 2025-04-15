#include "PhSensor.h"

PhSensor::PhSensor(int pin, float caliVal, int redLed, int greenLed, int blueLed, int buzzer,
                   LiquidCrystal_I2C& lcd, WiFiManager& wifiManager, PowerManager& powerManager)
  : pin(pin), caliVal(caliVal), redLed(redLed), greenLed(greenLed), blueLed(blueLed),
    buzzer(buzzer), lcd(lcd), wifiManager(wifiManager), powerManager(powerManager) {
  // Initialize pins
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  pinMode(buzzer, OUTPUT);

  // Force initial state
    if (!powerManager.isSystemOn()) {
        forcePowerOffUpdate();
    }
}

float PhSensor::readPh() {
  if (!powerManager.isSystemOn()) {
    lastPhValue = NAN;
    updateServer();
    return NAN;
  }

  lastPhValue = readRawPh();
  updateServer();
  return lastPhValue;
}

float PhSensor::readRawPh() {
  int buffer_arr[10];
  for (int i = 0; i < 10; i++) {
    buffer_arr[i] = analogRead(pin);
    delay(30);
  }

  // Sort and average (median of middle 6 values)
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (buffer_arr[i] > buffer_arr[j]) {
        int temp = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temp;
      }
    }
  }

  int avgval = 0;
  for (int i = 2; i < 8; i++) avgval += buffer_arr[i];
  avgval /= 6;

  float voltage = (float)avgval * 3.3 / 4095.0;
  float ph_act = -5.70 * voltage + caliVal;

  updateIndicators(ph_act);
  updateDisplay(ph_act);

  return ph_act;
}

void PhSensor::updateIndicators(float phValue) {
  if (!powerManager.isSystemOn()) {
    // Turn off all indicators when system is off
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, LOW);
    digitalWrite(blueLed, LOW);
    digitalWrite(buzzer, LOW);
    return;
  }

  if (phValue <= 6.5) {
    digitalWrite(redLed, HIGH);
    digitalWrite(greenLed, LOW);
    digitalWrite(blueLed, LOW);
    digitalWrite(buzzer, HIGH);
  } else if (phValue <= 7.5) {
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, HIGH);
    digitalWrite(blueLed, LOW);
    digitalWrite(buzzer, LOW);
  } else if (phValue > 7.5) {
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, LOW);
    digitalWrite(blueLed, HIGH);
    digitalWrite(buzzer, HIGH);
  }
}

void PhSensor::updateDisplay(float phValue) {
  if (!powerManager.isSystemOn()) {
    // lcd.setCursor(0, 0);
    // lcd.print("PH: -- OFF --");
    return;
  }

  lcd.setCursor(0, 0);
  lcd.print("PH:");
  lcd.setCursor(3, 0);
  if (isnan(phValue)) {
    lcd.print("-- ERR --");
  } else {
    lcd.print(phValue, 2);
  }
}

void PhSensor::updateServer() {
  // Add immediate return if WiFi isn't connected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected - can't update server");
    return;
  }

  String body;
  if (!powerManager.isSystemOn()) {
    body = "phSensor=-&powerState=off";
  } else if (isnan(lastPhValue)) {
    body = "phSensor=err&powerState=on";
  } else {
    body = "phSensor=" + String(lastPhValue, 2) + "&powerState=on";
  }

  // Add timestamp to debug power state changes
  // Serial.print(millis());
  // Serial.print(" - ");
  // Serial.println("Sending: " + body);

  HTTPClient http;
  http.begin(serverURL);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST(body);

  if (httpCode > 0) {
    Serial.printf("HTTP Code: %d\n", httpCode);
  } else {
    Serial.printf("HTTP Error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}
void PhSensor::sendPhToServer(float phValue) {
  lastPhValue = phValue;
  updateServer();
}