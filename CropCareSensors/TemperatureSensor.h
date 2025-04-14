#ifndef TEMPERATURE_SENSOR_H  // include guards to avoid duplicate definitions
#define TEMPERATURE_SENSOR_H

#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>

class TemperatureSensor {  // creates class named TemperatureSensor
private:                   // private objects so that we can only use it internally
  int pin;
  OneWire oneWire;
  DallasTemperature sensors;
  LiquidCrystal_I2C& lcd;

  const char* ssid = "THE MAGOLLADOS'S";
  const char* password = "DEMO POWER";
  const char* serverURL = "http://192.168.100.42:3000/update_temp.php";  // Your PHP endpoint

public:                                                // public objects so that we can call it outside the class
  TemperatureSensor(int pin, LiquidCrystal_I2C& lcd);  // A constructor to pass the GPIO pin came from the user or initialized it from the main
  void begin();                                        // Initialize the temperature sensor
  float getCelsius();                                  // Getter for returning the value celcius
  float getFahrenheit();                               // Getter for returning the value fahrenheit
  void lcdTemperatureSensor();

  void connectWiFi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("WiFi connected");
  }
void sendTemperatureToServer(float temperature) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String body = "tempSensor=" + String(temperature, 2);

    Serial.println("Sending POST: " + body);  // 🪵 Debug info

    int code = http.POST(body);
    String response = http.getString(); // 🪵 Get the response from server

    Serial.printf("POST returned %d\n", code);
    Serial.println("Server response: " + response);

    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
}


};

#endif
