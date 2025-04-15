#include "PhSensor.h"

PhSensor::PhSensor(int pin, float caliVal, int redLed, int greenLed, int blueLed, int buzzer, LiquidCrystal_I2C& lcd, WiFiManager& wifiManager)
  : pin(pin), caliVal(caliVal), redLed(redLed), greenLed(greenLed), blueLed(blueLed), buzzer(buzzer), lcd(lcd), wifiManager(wifiManager) {
  // Initialize pins
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  pinMode(buzzer, OUTPUT);
}

void PhSensor::powerOff() {
  isPoweredOn = false;
  // Turn off all indicators
  digitalWrite(redLed, LOW);
  digitalWrite(greenLed, LOW);
  digitalWrite(blueLed, LOW);
  digitalWrite(buzzer, LOW);

  // Send "off" state to server
  updateServer();
  // Immediately send "off" state to server
  lastPhValue = NAN;  // Force off state
  updateServer();

  // Add delay to ensure message is sent
  delay(100);
}

void PhSensor::powerOn() {
  isPoweredOn = true;
  // Indicators will update during next read
}

float PhSensor::readPh() {
  lastPhValue = readRawPh();  // Store the value regardless of power state
  updateServer();             // Always update server with current state
  return lastPhValue;
}

float PhSensor::readRawPh() {
  if (!isPoweredOn) {
    return NAN;
  }

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
  if (!isPoweredOn) return;

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
  if (!isPoweredOn) {
    lcd.setCursor(0, 0);
    lcd.print("PH: -- OFF --");
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
  String body;
  if (!isPoweredOn) {
    body = "phSensor=-&powerState=off";
  } else if (isnan(lastPhValue)) {
    body = "phSensor=err&powerState=on";
  } else {
    body = "phSensor=" + String(lastPhValue, 2) + "&powerState=on";
  }

  wifiManager.sendHTTPPost(serverURL, body);
  Serial.print("Sending " + body);
}

void PhSensor::sendPhToServer(float phValue) {
  lastPhValue = phValue;  // Store the value
  updateServer();         // Update server with current state
}