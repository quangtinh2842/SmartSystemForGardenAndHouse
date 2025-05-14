#define TEMP_AND_HUMI_PIN 13
#define SOUND_PIN 26
#define FAN_BUTTON_PIN 27
#define LIGHT_BUTTON_PIN 18
#define HEATER_BUTTON_PIN 17
#define HUMIDIFIER_BUTTON_PIN 16

#define SOUND_LED_PIN 25
#define FAN_RELAY_PIN 21
#define LIGHT_RELAY_PIN 19
#define HEATER_RELAY_PIN 22
#define HUMIDIFIER_RELAY_PIN 23

#define BROADCASTING_LED_PIN 2

#define MESH_PREFIX "MESH_PREFIX_SSFGAH"
#define MESH_PASSWORD "8ajk@aj&ma(kj'a9"
#define MESH_PORT 5555

#define ROOT_NODE_ID 3765139098
#define LIVING_ROOM_NODE_ID 3940552969
#define BEDROOM_NODE_ID 4240944617
#define KITCHEN_NODE_ID 4239064149
#define GARDEN_NODE_ID 1819790961

#define DHTTYPE DHT11

#include <Button.h>
#include <WiFi.h>
#include "painlessMesh.h"
#include "BClapSwitch.h"
#include "SPackage.h"
#include "DHT.h"

Scheduler scheduler;
painlessMesh mesh;
void messageBroadcastingScheduled();
void receivedBroadcastingMsgCallback(uint32_t from, String &msg);
Task taskBroadcastMessage((TASK_SECOND*10), TASK_FOREVER, &messageBroadcastingScheduled);
DHT dht11(TEMP_AND_HUMI_PIN, DHTTYPE);
Button lightBtn(LIGHT_BUTTON_PIN);
Button fanBtn(FAN_BUTTON_PIN);
Button heaterBtn(HEATER_BUTTON_PIN);
Button humidifierBtn(HUMIDIFIER_BUTTON_PIN);
BClapSwitch clapSwitch;
SPackage firstPackage;
JsonDocument firstDoc;
SPackage secondPackage;
JsonDocument secondDoc;

void setup() {
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &scheduler, MESH_PORT);
  mesh.onReceive(&receivedBroadcastingMsgCallback);
  scheduler.addTask(taskBroadcastMessage);
  taskBroadcastMessage.enable();
  dht11.begin();
  clapSwitch.begin(SOUND_PIN, SOUND_LED_PIN);
  lightBtn.begin();
  fanBtn.begin();
  heaterBtn.begin();
  humidifierBtn.begin();
  pinMode(BROADCASTING_LED_PIN, OUTPUT);
  digitalWrite(BROADCASTING_LED_PIN, HIGH);
  pinMode(LIGHT_RELAY_PIN, OUTPUT);
  pinMode(FAN_RELAY_PIN, OUTPUT);
  pinMode(HEATER_RELAY_PIN, OUTPUT);
  pinMode(HUMIDIFIER_RELAY_PIN, OUTPUT);
  digitalWrite(LIGHT_RELAY_PIN, HIGH);
  digitalWrite(FAN_RELAY_PIN, HIGH);
  digitalWrite(HEATER_RELAY_PIN, HIGH);
  digitalWrite(HUMIDIFIER_RELAY_PIN, HIGH);
  delay(1000);
}

unsigned long lastUpdatingTAndHMillis = 0;
int temp = 0;
int humi = 0;

bool wasFanOn = false;
uint8_t fanTempThresh = 23;
bool isFanAutoModeOn = true;

bool wasHeaterOn = false;
uint8_t heaterTempThresh = 18;
bool isHeaterAutoModeOn = true;

bool wasHumidifierOn = false;
uint8_t humidifierHumiThresh = 40;
bool isHumidifierAutoModeOn = true;

bool isClapSwitchModeOn = true;

void loop() {
  mesh.update();

  if (millis() - lastUpdatingTAndHMillis > 9000) {
    temp = dht11.readTemperature();
    humi = dht11.readHumidity();
    lastUpdatingTAndHMillis = millis();
  }

  clapSwitch.listenAndHandle(clapSwitchHandle);

  if (lightBtn.pressed()) {
    digitalWrite(LIGHT_RELAY_PIN, !digitalRead(LIGHT_RELAY_PIN));
    firstPackage.pieceArray[0].updatePieceFrom("203", String(getLightState()));
    broadcastFirstPackageToRootWithNumFirstPieces(1);
  }

  if (isFanAutoModeOn && temp > fanTempThresh) {
    if (wasFanOn == false) {
      wasFanOn = true;
      digitalWrite(FAN_RELAY_PIN, LOW);
      firstPackage.pieceArray[0].updatePieceFrom("205", String(getFanState()));
      broadcastFirstPackageToRootWithNumFirstPieces(1);
    }
  } else if (isFanAutoModeOn && temp <= fanTempThresh) {
    if (wasFanOn == true) {
      wasFanOn = false;
      digitalWrite(FAN_RELAY_PIN, HIGH);
      firstPackage.pieceArray[0].updatePieceFrom("205", String(getFanState()));
      broadcastFirstPackageToRootWithNumFirstPieces(1);
    }
  }

  if (fanBtn.pressed()) {
    digitalWrite(FAN_RELAY_PIN, !digitalRead(FAN_RELAY_PIN));
    isFanAutoModeOn = false;
    wasFanOn = getFanState();
    firstPackage.pieceArray[0].updatePieceFrom("205", String(getFanState()));
    firstPackage.pieceArray[1].updatePieceFrom("206", String(isFanAutoModeOn));
    broadcastFirstPackageToRootWithNumFirstPieces(2);
  }

  if (isHeaterAutoModeOn && temp < heaterTempThresh) {
    if (wasHeaterOn == false) {
      wasHeaterOn = true;
      digitalWrite(HEATER_RELAY_PIN, LOW);
      firstPackage.pieceArray[0].updatePieceFrom("208", String(getHeaterState()));
      broadcastFirstPackageToRootWithNumFirstPieces(1);
    }
  } else if (isHeaterAutoModeOn && temp >= heaterTempThresh) {
    if (wasHeaterOn == true) {
      wasHeaterOn = false;
      digitalWrite(HEATER_RELAY_PIN, HIGH);
      firstPackage.pieceArray[0].updatePieceFrom("208", String(getHeaterState()));
      broadcastFirstPackageToRootWithNumFirstPieces(1);
    }
  }

  if (heaterBtn.pressed()) {
    digitalWrite(HEATER_RELAY_PIN, !digitalRead(HEATER_RELAY_PIN));
    isHeaterAutoModeOn = false;
    wasHeaterOn = getHeaterState();
    firstPackage.pieceArray[0].updatePieceFrom("208", String(getHeaterState()));
    firstPackage.pieceArray[1].updatePieceFrom("209", String(isHeaterAutoModeOn));
    broadcastFirstPackageToRootWithNumFirstPieces(2);
  }

  if (isHumidifierAutoModeOn && humi < humidifierHumiThresh) {
    if (wasHumidifierOn == false) {
      wasHumidifierOn = true;
      digitalWrite(HUMIDIFIER_RELAY_PIN, LOW);
      firstPackage.pieceArray[0].updatePieceFrom("211", String(getHumidifierState()));
      broadcastFirstPackageToRootWithNumFirstPieces(1);
    }
  } else if (isHumidifierAutoModeOn && humi >= humidifierHumiThresh) {
    if (wasHumidifierOn == true) {
      wasHumidifierOn = false;
      digitalWrite(HUMIDIFIER_RELAY_PIN, HIGH);
      firstPackage.pieceArray[0].updatePieceFrom("211", String(getHumidifierState()));
      broadcastFirstPackageToRootWithNumFirstPieces(1);
    }
  }

  if (humidifierBtn.pressed()) {
    digitalWrite(HUMIDIFIER_RELAY_PIN, !digitalRead(HUMIDIFIER_RELAY_PIN));
    isHumidifierAutoModeOn = false;
    wasHumidifierOn = getHumidifierState();
    firstPackage.pieceArray[0].updatePieceFrom("211", String(getHumidifierState()));
    firstPackage.pieceArray[1].updatePieceFrom("212", String(isHumidifierAutoModeOn));
    broadcastFirstPackageToRootWithNumFirstPieces(2);
  }
}

void clapSwitchHandle(String type) {
  if (type == "252") {
    firstPackage.pieceArray[0].updatePieceFrom("202", "1");
    broadcastFirstPackageToRootWithNumFirstPieces(1);
  } else if (type == "TOGGLE_LIGHT") {
    if (isClapSwitchModeOn) {
      digitalWrite(LIGHT_RELAY_PIN, !digitalRead(LIGHT_RELAY_PIN));
      firstPackage.pieceArray[0].updatePieceFrom("203", String(getLightState()));
      broadcastFirstPackageToRootWithNumFirstPieces(1);
    }
  }
}

void messageBroadcastingScheduled() {
  firstPackage.pieceArray[0].updatePieceFrom("200", String(temp));
  firstPackage.pieceArray[1].updatePieceFrom("201", String(humi));
  firstPackage.pieceArray[2].updatePieceFrom("202", "0");
  firstPackage.pieceArray[3].updatePieceFrom("203", String(getLightState()));
  firstPackage.pieceArray[4].updatePieceFrom("204", String(isClapSwitchModeOn));
  firstPackage.pieceArray[5].updatePieceFrom("205", String(getFanState()));
  firstPackage.pieceArray[6].updatePieceFrom("206", String(isFanAutoModeOn));
  firstPackage.pieceArray[7].updatePieceFrom("207", String(fanTempThresh));
  firstPackage.pieceArray[8].updatePieceFrom("208", String(getHeaterState()));
  firstPackage.pieceArray[9].updatePieceFrom("209", String(isHeaterAutoModeOn));
  firstPackage.pieceArray[10].updatePieceFrom("210", String(heaterTempThresh));
  firstPackage.pieceArray[11].updatePieceFrom("211", String(getHumidifierState()));
  firstPackage.pieceArray[12].updatePieceFrom("212", String(isHumidifierAutoModeOn));
  firstPackage.pieceArray[13].updatePieceFrom("213", String(humidifierHumiThresh));
  broadcastFirstPackageToRootWithNumFirstPieces(14);
}

void receivedBroadcastingMsgCallback(uint32_t from, String &msg) {
  parseMsgToConsiderToUpdate(msg);
}

void parseMsgToConsiderToUpdate(String msg) {
  firstDoc.clear();
  deserializeJson(firstDoc, msg);
  uint32_t toID = firstDoc["to"].as<uint32_t>();
  if (toID == mesh.getNodeId()) {
    updateBedroomFrom(firstDoc);
  }
}

void updateBedroomFrom(JsonDocument docParam) {
  firstPackage.clearAllExceptPieceArray();
  firstPackage.updateFrom(docParam);
  for (uint8_t i = 0; i < firstPackage.nPieces; i++) {
    uint8_t value = firstPackage.pieceArray[i].value.toInt();
    if (firstPackage.pieceArray[i].id == "203") {
      digitalWrite(LIGHT_RELAY_PIN, !value);
    } else if (firstPackage.pieceArray[i].id == "204") {
      isClapSwitchModeOn = value;
    } else if (firstPackage.pieceArray[i].id == "205") {
      digitalWrite(FAN_RELAY_PIN, !value);
      isFanAutoModeOn = false;
      wasFanOn = value;
      secondPackage.pieceArray[0].updatePieceFrom("206", String(isFanAutoModeOn));
      broadcastSecondPackageToRootWithNumFirstPieces(1);
    } else if (firstPackage.pieceArray[i].id == "206") {
      isFanAutoModeOn = value;
    } else if (firstPackage.pieceArray[i].id == "207") {
      fanTempThresh = value;
    } else if (firstPackage.pieceArray[i].id == "208") {
      digitalWrite(HEATER_RELAY_PIN, !value);
      isHeaterAutoModeOn = false;
      wasHeaterOn = value;
      secondPackage.pieceArray[0].updatePieceFrom("209", String(isHeaterAutoModeOn));
      broadcastSecondPackageToRootWithNumFirstPieces(1);
    } else if (firstPackage.pieceArray[i].id == "209") {
      isHeaterAutoModeOn = value;
    } else if (firstPackage.pieceArray[i].id == "210") {
      heaterTempThresh = value;
    } else if (firstPackage.pieceArray[i].id == "211") {
      digitalWrite(HUMIDIFIER_RELAY_PIN, !value);
      isHumidifierAutoModeOn = false;
      wasHumidifierOn = value;
      secondPackage.pieceArray[0].updatePieceFrom("212", String(isHumidifierAutoModeOn));
      broadcastSecondPackageToRootWithNumFirstPieces(1);
    } else if (firstPackage.pieceArray[i].id == "212") {
      isHumidifierAutoModeOn = value;
    } else if (firstPackage.pieceArray[i].id == "213") {
      humidifierHumiThresh = value;
    }
  }
}

void broadcastFirstPackageToRootWithNumFirstPieces(uint8_t nPiecesParam) {
  firstPackage.clearAllExceptPieceArray();
  firstPackage.from = mesh.getNodeId();
  firstPackage.to = ROOT_NODE_ID;
  firstPackage.nPieces = nPiecesParam;
  firstDoc.clear();
  firstPackage.nestInto(firstDoc);
  broadcast(firstDoc.as<String>());
}

void broadcastSecondPackageToRootWithNumFirstPieces(uint8_t nPiecesParam) {
  secondPackage.clearAllExceptPieceArray();
  secondPackage.from = mesh.getNodeId();
  secondPackage.to = ROOT_NODE_ID;
  secondPackage.nPieces = nPiecesParam;
  secondDoc.clear();
  secondPackage.nestInto(secondDoc);
  broadcast(secondDoc.as<String>());
}

void broadcast(String msg) {
  digitalWrite(BROADCASTING_LED_PIN, !digitalRead(BROADCASTING_LED_PIN));
  mesh.sendBroadcast(msg);
}

bool getLightState() {
  return !digitalRead(LIGHT_RELAY_PIN);
}

bool getFanState() {
  return !digitalRead(FAN_RELAY_PIN);
}

bool getHeaterState() {
  return !digitalRead(HEATER_RELAY_PIN);
}

bool getHumidifierState() {
  return !digitalRead(HUMIDIFIER_RELAY_PIN);
}