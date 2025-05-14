// #define BLYNK_TEMPLATE_ID "TMPL6RJJJjThv"
// #define BLYNK_TEMPLATE_NAME "SSFGAH"
// #define BLYNK_AUTH_TOKEN "KzqIptRoAczXdALXc7zC6F5xOyf651KH"

#define BLYNK_TEMPLATE_ID "TMPL6QtbggWnN"
#define BLYNK_TEMPLATE_NAME "SSFGAH"
#define BLYNK_AUTH_TOKEN "cnfioCvbWzwqN_0QW0YraZixvrWYEu81"

#define LEFT_BUTTON_PIN D5
#define RIGHT_BUTTON_PIN D6
#define SWITCH_BUTTON_PIN D7
#define RESET_BUTTON_PIN D3

#define TRANSMITTED_LED_PIN D4

#define SCL_PIN D1
#define SDA_PIN D2

#define MODULE_NAME_VPIN V0
#define MONITOR_VPIN V1
#define PIECE_VPIN V2
#define NUMERICAL_ORDER_VPIN V3
#define SWITCH_VPIN V4

#define ROOT_NODE_ID 3765139098
#define LIVING_ROOM_NODE_ID 3940552969
#define BEDROOM_NODE_ID 4240944617
#define KITCHEN_NODE_ID 4239064149
#define GARDEN_NODE_ID 1819790961

#define SOUND_CAUSE_PRESS "SOUND_CAUSE_PRESS"

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Button.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "EEPROMHelper.h"
#include "HomePage.h"
#include "SetupData.h"
#include "Package.h"

LiquidCrystal_I2C lcd(0x27, 20, 4);
EEPROMHelper eepromHelper;
ESP8266WebServer webServer(80);
Button resetBtn(RESET_BUTTON_PIN);
Button leftBtn(LEFT_BUTTON_PIN);
Button rightBtn(RIGHT_BUTTON_PIN);
Button switchBtn(SWITCH_BUTTON_PIN);

Package tmpPackage;
bool wasSkipped = false;
unsigned long lastGotRequestMillis = 0;
uint8_t preNumericOrderValue;
bool isThreshChanging = false;
uint8_t numericOrderValue = 0;
uint8_t nPieces;
Piece pieceArray[40];
JsonDocument doc;

BLYNK_CONNECTED() {
  updateRow1();
  updateRow2();
  updateRow3();
  updateRow4();
}

BLYNK_WRITE(NUMERICAL_ORDER_VPIN) {
  uint8_t paramValue = param.asInt();
  if ((paramValue < nPieces) || (isThreshChanging && (paramValue < 100))) {
    numericOrderValue = paramValue;
    if (isThreshChanging) {
      updateRow4();
    } else {
      updateRow1();
      updateRow2();
      updateRow3();
      updateRow4();
    }
  } else {
    Blynk.virtualWrite(NUMERICAL_ORDER_VPIN, numericOrderValue);
  }
}

BLYNK_WRITE(SWITCH_VPIN) {
  switchTappedOrChanged();
}

void setup() {
  lcd.init();
  lcd.backlight();
  eepromHelper.begin();
  checkAndSetupWiFi();
  Serial.begin(9600);
  setupPieceArray();
  resetBtn.begin();
  leftBtn.begin();
  rightBtn.begin();
  switchBtn.begin();
  pinMode(TRANSMITTED_LED_PIN, OUTPUT);
  digitalWrite(TRANSMITTED_LED_PIN, HIGH);
  delay(1000);
  updateRow1();
  updateRow2();
  updateRow3();
  updateRow4();
  Blynk.config(BLYNK_AUTH_TOKEN);
}

void loop() {
  Blynk.run();

  if (leftBtn.pressed()) {
    transmit(SOUND_CAUSE_PRESS);
    if (numericOrderValue > 0) {
      numericOrderValue -= 1;
      if (isThreshChanging) {
        updateRow4();
      } else {
        updateRow1();
        updateRow2();
        updateRow3();
        updateRow4();
      }
    }
  }

  if (rightBtn.pressed()) {
    transmit(SOUND_CAUSE_PRESS);
    if ((numericOrderValue < nPieces-1) || (isThreshChanging && (numericOrderValue < 100))) {
      numericOrderValue += 1;
      if (isThreshChanging) {
        updateRow4();
      } else {
        updateRow1();
        updateRow2();
        updateRow3();
        updateRow4();
      }
    }
  }

  if (switchBtn.pressed()) {
    transmit(SOUND_CAUSE_PRESS);
    switchTappedOrChanged();
  }

  if (Serial.available()) {
    String msg = Serial.readStringUntil('\n');
    updatePieceArrayAndTriggerEventFrom(msg);
  }

  if (resetBtn.pressed()) {
    eepromHelper.clear();
    delay(50);
    ESP.reset();
  }
}

void transmit(String msg) {
  digitalWrite(TRANSMITTED_LED_PIN, !digitalRead(TRANSMITTED_LED_PIN));
  Serial.println(msg);
}

void transmitCurrentPiece() {
  String fromModule = pieceArray[numericOrderValue].fromModule;
  tmpPackage.from = ROOT_NODE_ID;
  if (fromModule == "Living Room") {
    tmpPackage.to = LIVING_ROOM_NODE_ID;
  } else if (fromModule == "Bedroom") {
    tmpPackage.to = BEDROOM_NODE_ID;
  } else if (fromModule == "Kitchen") {
    tmpPackage.to = KITCHEN_NODE_ID;
  } else if (fromModule == "Garden") {
    tmpPackage.to = GARDEN_NODE_ID;
  }
  tmpPackage.nPieces = 1;
  tmpPackage.pieceArray[0] = pieceArray[numericOrderValue];
  doc.clear();
  tmpPackage.nestInto(doc);
  transmit(doc.as<String>());
  doc.clear();
}

void updatePieceArrayAndTriggerEventFrom(String msg) {
  doc.clear();
  deserializeJson(doc, msg);
  uint8_t count = doc["nPieces"].as<uint8_t>();
  for(uint8_t i = 0; i < count; i++) {
    for(uint8_t j = 0; j < nPieces; j++) {
      if (doc["pieceArray"][i]["id"].as<String>() == pieceArray[j].id) {
        pieceArray[j].updateFromPackagePieces(doc, i);
        if (pieceArray[j].type == EVENT_TYPE) {
          if (pieceArray[j].value == "1") {
            Blynk.logEvent(pieceArray[j].eventCode, "From "+pieceArray[j].fromModule);
          }
        }
        break;
      }
    }
  }
  doc.clear();
  if (isThreshChanging) {
    updateRow2();
  } else {
    updateRow2();
    updateRow3();
    updateRow4();
  }
}

void updateRow1() {
  String line = pieceArray[numericOrderValue].fromModule;
  Blynk.virtualWrite(MODULE_NAME_VPIN, line);
  lcd.setCursor(0, 0);
  lcd.print("                    ");
  lcd.setCursor(0, 0);
  lcd.print(line);
}

void updateRow2() {
  String fromModule = pieceArray[numericOrderValue].fromModule;
  String line = "";
  for (uint8_t i = 0; i < nPieces; i++) {
    if (pieceArray[i].fromModule == fromModule) {
      if (pieceArray[i].type == "SENSOR_TYPE" || pieceArray[i].type == "EVENT_TYPE") {
        line += pieceArray[i].sign + ":" + pieceArray[i].value + " ";
      }
    }
  }
  line.remove((line.length()-1));
  Blynk.virtualWrite(MONITOR_VPIN, line);
  lcd.setCursor(0, 1);
  lcd.print("                    ");
  lcd.setCursor(0, 1);
  lcd.print(line);
}

void updateRow3() {
  String line = pieceArray[numericOrderValue].name + ":" + (String)pieceArray[numericOrderValue].value;
  Blynk.virtualWrite(PIECE_VPIN, line);
  lcd.setCursor(0, 2);
  lcd.print("                    ");
  lcd.setCursor(0, 2);
  lcd.print(line);
}

void updateRow4() {
  lcd.setCursor(0, 3);
  lcd.print("                    ");
  lcd.setCursor(0, 3);
  lcd.print(String(numericOrderValue));

  uint8_t value = pieceArray[numericOrderValue].value.toInt();
  if (isThreshChanging) {
    lcd.setCursor(17, 3);
    lcd.print("ON");
    Blynk.virtualWrite(SWITCH_VPIN, 1);
  } else if (pieceArray[numericOrderValue].valueType == "INT_TYPE" && pieceArray[numericOrderValue].type == "CONTROL_TYPE" && !isThreshChanging) {
    lcd.setCursor(17, 3);
    lcd.print("OFF");
    Blynk.virtualWrite(SWITCH_VPIN, 0);
  } else if (pieceArray[numericOrderValue].valueType == "BOOL_TYPE") {
    lcd.setCursor(17, 3);
    String switchState = value == 0? "OFF": "ON";
    lcd.print(switchState);
    Blynk.virtualWrite(SWITCH_VPIN, value);
  }

  Blynk.virtualWrite(NUMERICAL_ORDER_VPIN, numericOrderValue);
}

void switchTappedOrChanged() {
  if (pieceArray[numericOrderValue].type == "CONTROL_TYPE" || isThreshChanging) {
    if (pieceArray[numericOrderValue].valueType == "BOOL_TYPE" && !isThreshChanging) {
      pieceArray[numericOrderValue].value = !pieceArray[numericOrderValue].value.toInt();
      updateRow3();
      transmitCurrentPiece();
    } else if (pieceArray[numericOrderValue].valueType == "INT_TYPE" || isThreshChanging) {
      if (isThreshChanging) {
        isThreshChanging = false;
        pieceArray[preNumericOrderValue].value = (String)numericOrderValue;
        numericOrderValue = preNumericOrderValue;
        updateRow3();
        transmitCurrentPiece();
      } else {
        isThreshChanging = true;
        preNumericOrderValue = numericOrderValue;
        numericOrderValue = pieceArray[numericOrderValue].value.toInt();
      }
    }
    updateRow4();
  } else {
    Blynk.virtualWrite(SWITCH_VPIN, 0);
  }
}

void setupPieceArray() {
  doc.clear();
  deserializeJson(doc, FPSTR(SETUP_DATA));
  nPieces = doc["nPieces"].as<uint8_t>();
  for (uint8_t i = 0; i < nPieces; i++) {
    pieceArray[i].id = doc["pieceArray"][i]["id"].as<String>();
    pieceArray[i].name = doc["pieceArray"][i]["name"].as<String>();
    pieceArray[i].sign = doc["pieceArray"][i]["sign"].as<String>();
    pieceArray[i].type = doc["pieceArray"][i]["type"].as<String>();
    pieceArray[i].valueType = doc["pieceArray"][i]["valueType"].as<String>();
    pieceArray[i].value = doc["pieceArray"][i]["value"].as<String>();
    pieceArray[i].fromModule = doc["pieceArray"][i]["fromModule"].as<String>();
    pieceArray[i].eventCode = doc["pieceArray"][i]["eventCode"].as<String>();
  }
  doc.clear();
}

void checkAndSetupWiFi() {
  if (eepromHelper.getSSID() == "") {
    String apSSID = "SSFGAH SETUP";
    String apPass = "k%f2#r92";
    WiFi.mode(WIFI_AP);
    WiFi.softAP(apSSID, apPass);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Wi-Fi SETUP");
    lcd.setCursor(0, 2);
    lcd.print("SSID: "+apSSID);
    lcd.setCursor(0, 3);
    lcd.print("PASS: "+apPass);
    webServer.on("/", handleHomePage);
    webServer.on("/handleSubmit", handleSubmit);
    webServer.on("/handleSkip", handleSkip);
    webServer.begin();
    while (1) {
      webServer.handleClient();

      // 5s after got request from client
      if (lastGotRequestMillis != 0 && lastGotRequestMillis < millis()) {
        if (wasSkipped) {
          webServer.close();
          webServer.stop();
          lcd.clear();
          break;
        } else {
          ESP.restart();
        }
      }
    }
  } else {
    String ssid = eepromHelper.getSSID();
    String pass = eepromHelper.getPass();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SAVED Wi-Fi");
    lcd.setCursor(0, 2);
    lcd.print("SSID: "+ssid);
    lcd.setCursor(0, 3);
    lcd.print("PASS: "+pass);
    delay(5000);
    lcd.clear();
    WiFi.begin(ssid, pass);
  }
}

void handleSubmit() {
  lastGotRequestMillis = (millis()+5000);
  String ssid = webServer.arg("ssid");
  String pass = webServer.arg("pass");
  eepromHelper.setSSID(ssid);
  eepromHelper.setPass(pass);
  String note = "Web Server got your submit request, SSID & PSK were saved.";
  webServer.send(200, "text/plain", note);
}

void handleSkip() {
  lastGotRequestMillis = (millis()+5000);
  wasSkipped = true;
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Wi-Fi SETUP SKIPPED");
  String note = "Web Server got your skip request.";
  webServer.send(200, "text/plain", note);
}

void handleHomePage() {
  webServer.send(200, "text/html", FPSTR(HOME_PAGE));
}