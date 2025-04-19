#include "PhSensor.h"

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

void PhSensor::begin() {
  if (!powerManager.isSystemOn() && WiFi.status() == WL_CONNECTED) {
    forcePowerOffUpdate();
  }
}

void PhSensor::readPhNonBlocking() {
  if (!powerManager.isSystemOn()) {
    lastPhValue = NAN;
    forcePowerOffUpdate();
    return;
  }

  if (!isSampling) {
    sampleIndex = 0;
    isSampling = true;
    lastReadTime = millis();

    lcd.setCursor(0, 0);
    lcd.print("PH:...");
    lcd.print("   ");
    lcd.setCursor(4, 0);
    Serial.println("Start reading pH...");
  }

  if (isSampling && millis() - lastReadTime >= readInterval) {
    lastReadTime = millis();
    buffer_arr[sampleIndex++] = analogRead(pin);

    if (sampleIndex >= 10) {
      isSampling = false;
      unsigned long processingStart = millis();

      // Sort and median average
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

      float voltage = (float)avgval * 2.5 / 4095.0;
      float ph_act = -5.70 * voltage + caliVal;
      lastPhValue = ph_act;

      Serial.print("Finished analog reads in: ");
      Serial.println(millis() - processingStart);

      lcd.setCursor(0, 0);
      lcd.print("PH:");
      lcd.print(ph_act, 1);
      lcd.print("   ");
      lcd.setCursor(8, 0);

      // Indicators without delay
      buzzerStartTime = 0;
      buzzerActive = false;

      if (ph_act <= 6.5) {
        digitalWrite(redLed, HIGH);
        digitalWrite(greenLed, LOW);
        digitalWrite(blueLed, LOW);
        digitalWrite(buzzer, HIGH);
        buzzerStartTime = millis();
        buzzerActive = true;
      } else if (ph_act <= 7.5) {
        digitalWrite(redLed, LOW);
        digitalWrite(greenLed, HIGH);
        digitalWrite(blueLed, LOW);
        digitalWrite(buzzer, LOW);
      } else {
        digitalWrite(redLed, LOW);
        digitalWrite(greenLed, LOW);
        digitalWrite(blueLed, HIGH);
        digitalWrite(buzzer, HIGH);
        buzzerStartTime = millis();
        buzzerActive = true;
      }

      processingStart = millis();
      sendPhToServer(ph_act);
      Serial.print("Sent to server in: ");
      Serial.println(millis() - processingStart);
    }
  }

  // Handle buzzer timing
  if (buzzerActive && millis() - buzzerStartTime >= 200) {
    digitalWrite(buzzer, LOW);
    buzzerActive = false;
  }
}

void PhSensor::sendPhToServer(float phValue) {
  String body = "phSensor=" + String(phValue, 2) + "&powerState=on";
  wifiManager.sendHTTPPost(serverURL, body);
}

void PhSensor::forcePowerOffUpdate() {
  String body = "phSensor=-&powerState=off";
  wifiManager.sendHTTPPost(serverURL, body);
}
