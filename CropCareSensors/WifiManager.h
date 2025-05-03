#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <BeepSound.h>
#include <WiFiClientSecure.h>
#include "SensorData.h" 
class WiFiManager {
private:
  const char* ssid;
  const char* password;
  LiquidCrystal_I2C* lcd;
  BeepSound* beepSound = nullptr;

public:
  WiFiManager(const char* ssid, const char* password)
    : ssid(ssid), password(password) {
  }
  void setBeepSound(BeepSound* buzzer) {
    beepSound = buzzer;
  }

  void setLCD(LiquidCrystal_I2C* lcdDisplay) {
    lcd = lcdDisplay;
  }

  void connect(unsigned long timeoutMs = 6000) {
    WiFi.begin(ssid, password);
    unsigned long startAttemptTime = millis();

    Serial.print("Connecting to WiFi");

    if (lcd) {
      lcd->clear();
      lcd->setCursor(0, 0);
      lcd->print("Connecting WiFi");
    }

    int dotCount = 0;

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeoutMs) {
      delay(500);
      Serial.print(".");
      if (lcd) {
        lcd->setCursor(dotCount % 16, 1);
        lcd->print(".");
        dotCount++;
      }
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWiFi connected");

      if (beepSound) beepSound->beepBuzzer(150, 1);  //  success beep

      if (lcd) {
        lcd->clear();
        lcd->setCursor(4, 0);
        lcd->print("Status:");
        lcd->setCursor(1, 1);
        lcd->print("WiFi Connected");
        delay(1000);
        lcd->noBacklight();
      }
    } else {
      Serial.println("\nWiFi connection failed, proceeding in offline mode.");

      if (beepSound) beepSound->beepBuzzer(150, 3);  //  failure beeps

      if (lcd) {
        lcd->clear();
        lcd->setCursor(4, 0);
        lcd->print("Status:");
        lcd->setCursor(0, 1);
        lcd->print("WiFi Not Connected");
        delay(1000);
        lcd->noBacklight();
      }
    }

    delay(2000);
  }

  bool isConnected() {
    return WiFi.status() == WL_CONNECTED;
  }
  void sendAllSensorData(const char* serverURL, const SensorData& data) {
  if (!isConnected()) {
    Serial.println("WiFi not connected");
    return;
  }

  String body = "phSensor=" + String(data.phValue, 2) + 
                "&moistureSensor=" + String(data.moisturePercent) +
                "&temperature=" + String(data.temperature, 2) +
                "&humidity=" + String(data.humidity) +
                "&powerState=" + data.powerState;

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  http.begin(serverURL);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Connection", "close");

  int code = http.POST(body);
  String response = http.getString();

  Serial.print("HTTP Response code: ");
  Serial.println(code);
  Serial.print("Server response: ");
  Serial.println(response);

  http.end();
}
};
#endif