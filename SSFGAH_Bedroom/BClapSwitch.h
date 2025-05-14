struct BClapSwitch {
  uint8_t soundPin;
  uint8_t soundLEDPin;
  unsigned long claps[9];
  uint8_t index = 0;
  unsigned long last = 0;

  bool is252() {
    if (claps[8]-claps[7] > 1000) {
      return false;
    }

    if (claps[7]-claps[6] <= 1000) {
      return false;
    }

    if (claps[6]-claps[5] > 1000) {
      return false;
    }

    if (claps[5]-claps[4] > 1000) {
      return false;
    }

    if (claps[4]-claps[3] > 1000) {
      return false;
    }

    if (claps[3]-claps[2] > 1000) {
      return false;
    }

    if (claps[2]-claps[1] <= 1000) {
      return false;
    }

    if (claps[1]-claps[0] > 1000) {
      return false;
    }

    return true;
  }

  bool isLightToggle() {
    if (index == 3) {
      if (claps[1]-claps[0] < 1000 && claps[2]-claps[1] > 1000) {
        index = 0;
        return true;
      }
    }
    return false;
  }

  void begin(uint8_t soundPinParam, uint8_t soundLEDPinParam) {
    this->soundPin = soundPinParam;
    this->soundLEDPin = soundLEDPinParam;
    pinMode(soundPin, INPUT);
    pinMode(soundLEDPin, OUTPUT);
  }

  void listenAndHandle(void (*func)(String type)) {
    if (millis()-last > 3500) {
      if (isLightToggle()) {
        func("TOGGLE_LIGHT");
      }
      index = 0;
    }

    if (digitalRead(SOUND_PIN) && millis()-last > 250) {
      last = millis();

      if (index > 8) {
        index = 0;
      }
      claps[index] = last;

      if (is252() && index == 8) {
        func("252");
        index = 0;
      } else {
        index += 1;
      }
      func(String(index));
    }

    if (millis()-last <= 250) {
      digitalWrite(SOUND_LED_PIN, HIGH);
    } else {
      digitalWrite(SOUND_LED_PIN, LOW);
    }
  }
};