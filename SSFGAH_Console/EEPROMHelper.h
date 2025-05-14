#define SSID_MAX_LENGTH 32
#define PASSWORD_MAX_LENGTH 64

#include <EEPROM.h>

class EEPROMHelper {
private:
  String ssid = "";
  String pass = "";
public:
  EEPROMHelper() {}

  void begin() {
    EEPROM.begin(512);
  }

  void setSSID(String ssid) {
    uint16_t startIndex = 0;
    uint16_t lastIndex = SSID_MAX_LENGTH-1;
    for (uint16_t i = startIndex; i <= lastIndex; i++) {
      EEPROM.write(i, ssid[i]);
      delay(10);
    }
    EEPROM.commit();
    this->ssid = ssid;
  }

  void setPass(String pass) {
    uint16_t startIndex = SSID_MAX_LENGTH;
    uint16_t lastIndex = SSID_MAX_LENGTH+PASSWORD_MAX_LENGTH-1;
    for (uint16_t i = startIndex; i <= lastIndex; i++) {
      EEPROM.write(i, pass[i-startIndex]);
      delay(10);
    }
    EEPROM.commit();
    this->pass = pass;
  }

  String getSSID() {
    if (this->ssid != "") { return this->ssid; }

    String ssid = "";
    uint16_t startIndex = 0;
    uint16_t lastIndex = SSID_MAX_LENGTH-1;
    for (uint16_t i = startIndex; i <= lastIndex; i++) {
      uint8_t value = EEPROM.read(i);
      if (value != 0) {
        ssid += char(value);
      } else {
        break;
      }
    }

    this->ssid = ssid;

    return ssid;
  }

  String getPass() {
    if (this->pass != "") { return this->pass; }

    String pass = "";
    uint16_t startIndex = SSID_MAX_LENGTH;
    uint16_t lastIndex = SSID_MAX_LENGTH+PASSWORD_MAX_LENGTH-1;
    for (uint16_t i = startIndex; i <= lastIndex; i++) {
      uint8_t value = EEPROM.read(i);
      if (value != 0) {
        pass += char(value);
      } else {
        break;
      }
    }

    this->pass = pass;

    return pass;
  }

  void clear() {
    for (uint16_t i = 0; i < 512; i++) {
      EEPROM.write(i, 0);           
      delay(10);
    }
    EEPROM.commit();
  }
};