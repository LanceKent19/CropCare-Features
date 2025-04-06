#ifndef TOGGLE_BUTTON_H
#define TOGGLE_BUTTON_H

#include <Arduino.h>

class ToggleButton {
private:
    int buttonPin;
    int ledPin;
    int lastButtonState;
    int ledState;
    unsigned long lastDebounceTime;
    const unsigned long debounceDelay;
public:
    ToggleButton(int btnPin, int ledPin) 
      : buttonPin(btnPin), ledPin(ledPin), debounceDelay(50) {
        ledState = LOW;
        lastDebounceTime = 0;
        lastButtonState = HIGH;
    }

    void begin() {
        pinMode(buttonPin, INPUT);
        pinMode(ledPin, OUTPUT);
        digitalWrite(ledPin, ledState);
    }

    bool update() {
        int reading = digitalRead(buttonPin);
        if (reading != lastButtonState) {
            lastDebounceTime = millis();
        }

        if ((millis() - lastDebounceTime) > debounceDelay) {
            if (reading == LOW && lastButtonState == HIGH) {
                ledState = !ledState;
                digitalWrite(ledPin, ledState);
                Serial.println(ledState == HIGH ? "Sensor On" : "Sensor Off");
            }
        }

        lastButtonState = reading;
        return ledState == HIGH;
    }
};

#endif
