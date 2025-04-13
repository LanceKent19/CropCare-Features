#ifndef BEEP_SOUND_H
#define BEEP_SOUND_H

class BeepSound {
private:
  int buzzerPin;

public:
  BeepSound(int buzzerPin)
    : buzzerPin(buzzerPin) {}

  void beepBuzzer(int duration, int count) {
    for (int i = 0; i < count; i++) {
      digitalWrite(buzzerPin, HIGH);
      delay(duration);
      digitalWrite(buzzerPin, LOW);
      delay(100);  // short pause between beeps
    }
  }
};

#endif