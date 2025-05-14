#define SOUND_PIN 25
#define GAS_PIN 26
#define SMOKE_PIN 27
#define LIGHT_1_BUTTON_PIN 16
#define LIGHT_2_BUTTON_PIN 17

#define SOUND_LED_PIN 32
#define BUZZER_PIN 18
#define LIGHT_1_RELAY_PIN 19
#define LIGHT_2_RELAY_PIN 21
#define SMOKE_RELAY_PIN 22
#define GAS_RELAY_PIN 23

#define BROADCASTING_LED_PIN 2

#define MESH_PREFIX "MESH_PREFIX_SSFGAH"
#define MESH_PASSWORD "8ajk@aj&ma(kj'a9"
#define MESH_PORT 5555

#define ROOT_NODE_ID 3765139098
#define LIVING_ROOM_NODE_ID 3940552969
#define BEDROOM_NODE_ID 4240944617
#define KITCHEN_NODE_ID 4239064149
#define GARDEN_NODE_ID 1819790961

#include <Button.h>
#include <WiFi.h>
#include "painlessMesh.h"
#include "KClapSwitch.h"
#include "SPackage.h"

Scheduler scheduler;
painlessMesh mesh;
void messageBroadcastingScheduled();
void receivedBroadcastingMsgCallback(uint32_t from, String &msg);
Task taskSendMessage((TASK_SECOND*10), TASK_FOREVER, &messageBroadcastingScheduled);
Button light1Btn(LIGHT_1_BUTTON_PIN);
Button light2Btn(LIGHT_2_BUTTON_PIN);
KClapSwitch clapSwitch;
SPackage tmpPackage;
JsonDocument tmpDoc;

void setup() {
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &scheduler, MESH_PORT);
  mesh.onReceive(&receivedBroadcastingMsgCallback);
  scheduler.addTask(taskSendMessage);
  taskSendMessage.enable();
  light1Btn.begin();
  light2Btn.begin();
  clapSwitch.begin(SOUND_PIN, SOUND_LED_PIN);
  pinMode(GAS_PIN, INPUT);
  pinMode(SMOKE_PIN, INPUT);
  pinMode(BROADCASTING_LED_PIN, OUTPUT);
  digitalWrite(BROADCASTING_LED_PIN, HIGH);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LIGHT_1_RELAY_PIN, OUTPUT);
  pinMode(LIGHT_2_RELAY_PIN, OUTPUT);
  pinMode(SMOKE_RELAY_PIN, OUTPUT);
  pinMode(GAS_RELAY_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LIGHT_1_RELAY_PIN, HIGH);
  digitalWrite(LIGHT_2_RELAY_PIN, HIGH);
  digitalWrite(SMOKE_RELAY_PIN, HIGH);
  digitalWrite(GAS_RELAY_PIN, HIGH);
  delay(2*60*1000);
}

bool wasSmokeDetected = false;
bool wasGasDetected = false;
bool isClapSwitchModeOn = true;

void loop() {
  mesh.update();

  clapSwitch.listenAndHandle(clapSwitchHandle);

  if (light1Btn.pressed()) {
    digitalWrite(LIGHT_1_RELAY_PIN, !digitalRead(LIGHT_1_RELAY_PIN));
    tmpPackage.pieceArray[0].updatePieceFrom("303", String(getLight1State()));
    broadcastTmpPackageToRootWithNumFirstPieces(1);
  }

  if (light2Btn.pressed()) {
    digitalWrite(LIGHT_2_RELAY_PIN, !digitalRead(LIGHT_2_RELAY_PIN));
    tmpPackage.pieceArray[0].updatePieceFrom("304", String(getLight2State()));
    broadcastTmpPackageToRootWithNumFirstPieces(1);
  }

  if (isSmoke()) {
    if (wasSmokeDetected == false) {
      wasSmokeDetected = true;
      digitalWrite(BUZZER_PIN, HIGH);
      digitalWrite(SMOKE_RELAY_PIN, LOW);
      tmpPackage.pieceArray[0].updatePieceFrom("300", String(isSmoke()));
      broadcastTmpPackageToRootWithNumFirstPieces(1);
    }
  } else {
    if (wasSmokeDetected == true) {
      wasSmokeDetected = false;
      digitalWrite(BUZZER_PIN, LOW);
      digitalWrite(SMOKE_RELAY_PIN, HIGH);
      tmpPackage.pieceArray[0].updatePieceFrom("300", String(isSmoke()));
      broadcastTmpPackageToRootWithNumFirstPieces(1);
    }
  }

  if (isGas()) {
    if (wasGasDetected == false) {
      wasGasDetected = true;
      digitalWrite(BUZZER_PIN, HIGH);
      digitalWrite(GAS_RELAY_PIN, LOW);
      tmpPackage.pieceArray[0].updatePieceFrom("301", String(isGas()));
      broadcastTmpPackageToRootWithNumFirstPieces(1);
    }
  } else {
    if (wasGasDetected == true) {
      wasGasDetected = false;
      digitalWrite(BUZZER_PIN, LOW);
      digitalWrite(GAS_RELAY_PIN, HIGH);
      tmpPackage.pieceArray[0].updatePieceFrom("301", String(isGas()));
      broadcastTmpPackageToRootWithNumFirstPieces(1);
    }
  }
}

void clapSwitchHandle(String type) {
  if (type == "252") {
    tmpPackage.pieceArray[0].updatePieceFrom("302", "1");
    broadcastTmpPackageToRootWithNumFirstPieces(1);
  } else if (type == "TOGGLE_LIGHT_1") {
    digitalWrite(LIGHT_1_RELAY_PIN, !digitalRead(LIGHT_1_RELAY_PIN));
    tmpPackage.pieceArray[0].updatePieceFrom("303", String(getLight1State()));
    broadcastTmpPackageToRootWithNumFirstPieces(1);
  } else if (type == "TOGGLE_LIGHT_2") {
    digitalWrite(LIGHT_2_RELAY_PIN, !digitalRead(LIGHT_2_RELAY_PIN));
    tmpPackage.pieceArray[0].updatePieceFrom("304", String(getLight2State()));
    broadcastTmpPackageToRootWithNumFirstPieces(1);
  }
}

void messageBroadcastingScheduled() {
  tmpPackage.pieceArray[0].updatePieceFrom("300", String(isSmoke()));
  tmpPackage.pieceArray[1].updatePieceFrom("301", String(isGas()));
  tmpPackage.pieceArray[2].updatePieceFrom("302", "0");
  tmpPackage.pieceArray[3].updatePieceFrom("303", String(getLight1State()));
  tmpPackage.pieceArray[4].updatePieceFrom("304", String(getLight2State()));
  tmpPackage.pieceArray[5].updatePieceFrom("305", String(isClapSwitchModeOn));
  broadcastTmpPackageToRootWithNumFirstPieces(6);
}

void receivedBroadcastingMsgCallback(uint32_t from, String &msg) {
  parseMsgToConsiderToUpdate(msg);
}

void parseMsgToConsiderToUpdate(String msg) {
  tmpDoc.clear();
  deserializeJson(tmpDoc, msg);
  uint32_t toID = tmpDoc["to"].as<uint32_t>();
  if (toID == mesh.getNodeId()) {
    updateKitchenFrom(tmpDoc);
  }
}

void updateKitchenFrom(JsonDocument docParam) {
  tmpPackage.clearAllExceptPieceArray();
  tmpPackage.updateFrom(docParam);
  for (uint8_t i = 0; i < tmpPackage.nPieces; i++) {
    uint8_t value = tmpPackage.pieceArray[i].value.toInt();
    if (tmpPackage.pieceArray[i].id == "303") {
      digitalWrite(LIGHT_1_RELAY_PIN, !value);
    } else if (tmpPackage.pieceArray[i].id == "304") {
      digitalWrite(LIGHT_2_RELAY_PIN, !value);
    } else if (tmpPackage.pieceArray[i].id == "305") {
      isClapSwitchModeOn = value;
    }
  }
}

void broadcastTmpPackageToRootWithNumFirstPieces(uint8_t nPiecesParam) {
  tmpPackage.clearAllExceptPieceArray();
  tmpPackage.from = mesh.getNodeId();
  tmpPackage.to = ROOT_NODE_ID;
  tmpPackage.nPieces = nPiecesParam;
  tmpDoc.clear();
  tmpPackage.nestInto(tmpDoc);
  broadcast(tmpDoc.as<String>());
}

void broadcast(String msg) {
  digitalWrite(BROADCASTING_LED_PIN, !digitalRead(BROADCASTING_LED_PIN));
  mesh.sendBroadcast(msg);
}

bool isGas() {
  return !digitalRead(GAS_PIN);
}

bool isSmoke() {
  return !digitalRead(SMOKE_PIN);
}

bool getLight1State() {
  return !digitalRead(LIGHT_1_RELAY_PIN);
}

bool getLight2State() {
  return !digitalRead(LIGHT_2_RELAY_PIN);
}