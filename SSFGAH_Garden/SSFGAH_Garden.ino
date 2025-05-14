#define MOTION_PIN 16
#define RAIN_PIN 17
#define LIGHT_BUTTON_PIN 18

#define BUZZER_PIN 19
#define LIGHT_RELAY_PIN 21
#define MOTION_RELAY_PIN 13

#define BROADCASTING_LED_PIN 2

#define MESH_PREFIX "MESH_PREFIX_SSFGAH"
#define MESH_PASSWORD "8ajk@aj&ma(kj'a9"
#define MESH_PORT 5555

#define ROOT_NODE_ID 3765139098
#define LIVING_ROOM_NODE_ID 3940552969
#define BEDROOM_NODE_ID 4240944617
#define KITCHEN_NODE_ID 4239064149
#define GARDEN_NODE_ID 1819790961

#include "painlessMesh.h"
#include "Button.h"
#include "SPackage.h"

Scheduler scheduler;
painlessMesh mesh;
void messageBroadcastingScheduled();
Task taskBroadcastMessage((TASK_SECOND*10), TASK_FOREVER, &messageBroadcastingScheduled);
Button lightBtn(LIGHT_BUTTON_PIN);
SPackage firstPackage;
JsonDocument firstDoc;
SPackage secondPackage;
JsonDocument secondDoc;

void setup() {
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &scheduler, MESH_PORT);
  mesh.onReceive(&receivedBroadcastingMsgCallback);
  scheduler.addTask(taskBroadcastMessage);
  taskBroadcastMessage.enable();
  lightBtn.begin();
  pinMode(BROADCASTING_LED_PIN, OUTPUT);
  digitalWrite(BROADCASTING_LED_PIN, HIGH);
  pinMode(MOTION_PIN, INPUT);
  pinMode(RAIN_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LIGHT_RELAY_PIN, OUTPUT);
  pinMode(MOTION_RELAY_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LIGHT_RELAY_PIN, HIGH);
  digitalWrite(MOTION_RELAY_PIN, HIGH);
  delay(2*60*1000);
}

bool wasThiefDetected = false;
bool wasRainDetected = false;
bool isLightAutoModeOn = true;
bool wasLightOn = false;
bool hadLight = false;
bool wasHadLightVarGotValue = false;

void loop() {
  mesh.update();

  if (isMotion()) {
    if (wasThiefDetected == false) {
      wasThiefDetected = true;
      digitalWrite(BUZZER_PIN, HIGH);
      digitalWrite(MOTION_RELAY_PIN, LOW);
      firstPackage.pieceArray[0].updatePieceFrom("400", String(isMotion()));
      broadcastFirstPackageToRootWithNumFirstPieces(1);
    }
  } else {
    if (wasThiefDetected == true) {
      wasThiefDetected = false;
      digitalWrite(BUZZER_PIN, LOW);
      digitalWrite(MOTION_RELAY_PIN, HIGH);
      firstPackage.pieceArray[0].updatePieceFrom("400", String(isMotion()));
      broadcastFirstPackageToRootWithNumFirstPieces(1);
    }
  }

  if (isRain()) {
    if (wasRainDetected == false) {
      wasRainDetected = true;
      firstPackage.pieceArray[0].updatePieceFrom("401", String(isRain()));
      broadcastFirstPackageToRootWithNumFirstPieces(1);
    }
  } else {
    if (wasRainDetected == true) {
      wasRainDetected = false;
      firstPackage.pieceArray[0].updatePieceFrom("401", String(isRain()));
      broadcastFirstPackageToRootWithNumFirstPieces(1);
    }
  }

  if (wasHadLightVarGotValue && isLightAutoModeOn && hadLight) {
    if (wasLightOn == true) {
      wasLightOn = false;
      digitalWrite(LIGHT_RELAY_PIN, HIGH);
      firstPackage.pieceArray[0].updatePieceFrom("402", String(getLightState()));
      broadcastFirstPackageToRootWithNumFirstPieces(1);
    }
  } else if (wasHadLightVarGotValue && isLightAutoModeOn && !hadLight) {
    if (wasLightOn == false) {
      wasLightOn = true;
      digitalWrite(LIGHT_RELAY_PIN, LOW);
      firstPackage.pieceArray[0].updatePieceFrom("402", String(getLightState()));
      broadcastFirstPackageToRootWithNumFirstPieces(1);
    }
  }

  if (lightBtn.pressed()) {
    digitalWrite(LIGHT_RELAY_PIN, !digitalRead(LIGHT_RELAY_PIN));
    isLightAutoModeOn = false;
    wasLightOn = getLightState();
    firstPackage.pieceArray[0].updatePieceFrom("402", String(getLightState()));
    firstPackage.pieceArray[1].updatePieceFrom("403", String(isLightAutoModeOn));
    broadcastFirstPackageToRootWithNumFirstPieces(2);
  }
}

void messageBroadcastingScheduled() {
  firstPackage.pieceArray[0].updatePieceFrom("400", String(isMotion()));
  firstPackage.pieceArray[1].updatePieceFrom("401", String(isRain()));
  firstPackage.pieceArray[2].updatePieceFrom("402", String(getLightState()));
  firstPackage.pieceArray[3].updatePieceFrom("403", String(isLightAutoModeOn));
  broadcastFirstPackageToRootWithNumFirstPieces(4);
}

void receivedBroadcastingMsgCallback(uint32_t from, String &msg) {
  parseMsgToConsiderToUpdate(msg);
}

void parseMsgToConsiderToUpdate(String msg) {
  firstDoc.clear();
  deserializeJson(firstDoc, msg);
  uint32_t toID = firstDoc["to"].as<uint32_t>();
  if (toID == mesh.getNodeId()) {
    updateGardenFrom(firstDoc);
  }
}

void updateGardenFrom(JsonDocument docParam) {
  firstPackage.clearAllExceptPieceArray();
  firstPackage.updateFrom(docParam);
  for (uint8_t i = 0; i < firstPackage.nPieces; i++) {
    uint8_t value = firstPackage.pieceArray[i].value.toInt();
    if (firstPackage.pieceArray[i].id == "402") {
      digitalWrite(LIGHT_RELAY_PIN, !value);
      isLightAutoModeOn = false;
      wasLightOn = value;
      secondPackage.pieceArray[0].updatePieceFrom("403", String(isLightAutoModeOn));
      broadcastSecondPackageToRootWithNumFirstPieces(1);
    } else if (firstPackage.pieceArray[i].id == "403") {
      isLightAutoModeOn = value;
    } else if (firstPackage.pieceArray[i].id == "102") {
      hadLight = value;
      wasHadLightVarGotValue = true;
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

bool isMotion() {
  return digitalRead(MOTION_PIN);
}

bool isRain() {
  return !digitalRead(RAIN_PIN);
}

bool getLightState() {
  return !digitalRead(LIGHT_RELAY_PIN);
}