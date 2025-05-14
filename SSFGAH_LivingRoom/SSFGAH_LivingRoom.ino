#define LIGHT_PIN 32
#define MOTION_PIN 34
#define SOUND_PIN 35
#define LIGHT_1_BUTTON_PIN 23
#define LIGHT_2_BUTTON_PIN 22
#define PORCH_LIGHT_BUTTON_PIN 21

#define SOUND_LED_PIN 13
#define BUZZER_PIN 33
#define MOTION_RELAY_PIN 16
#define LIGHT_1_RELAY_PIN 19
#define LIGHT_2_RELAY_PIN 17
#define PORCH_LIGHT_RELAY_PIN 18

#define TRANSMITTING_AND_BROADCASTING_LED_PIN 2

#define MESH_PREFIX "MESH_PREFIX_SSFGAH"
#define MESH_PASSWORD "8ajk@aj&ma(kj'a9"
#define MESH_PORT 5555

#define SOUND_CAUSE_PRESS "SOUND_CAUSE_PRESS"

#define ROOT_NODE_ID 3765139098
#define LIVING_ROOM_NODE_ID 3940552969
#define BEDROOM_NODE_ID 4240944617
#define KITCHEN_NODE_ID 4239064149
#define GARDEN_NODE_ID 1819790961

#include <Button.h>
#include <WiFi.h>
#include "painlessMesh.h"
#include "LRClapSwitch.h"
#include "SPackage.h"

Scheduler scheduler;
painlessMesh mesh;
void messageBroadcastingScheduled();
void receivedBroadcastingMsgCallback(uint32_t from, String &msg);
Task taskBroadcastMessage((TASK_SECOND*10), TASK_FOREVER, &messageBroadcastingScheduled);
Button porchLightButton(PORCH_LIGHT_BUTTON_PIN);
Button light1Button(LIGHT_1_BUTTON_PIN);
Button light2Button(LIGHT_2_BUTTON_PIN);
LRClapSwitch clapSwitch;
JsonDocument firstDoc;
SPackage firstPackage;
JsonDocument secondDoc;
SPackage secondPackage;

void setup() {
  Serial.begin(9600);
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &scheduler, MESH_PORT);
  mesh.onReceive(&receivedBroadcastingMsgCallback);
  scheduler.addTask(taskBroadcastMessage);
  taskBroadcastMessage.enable();
  porchLightButton.begin();
  light1Button.begin();
  light2Button.begin();
  clapSwitch.begin(SOUND_PIN, SOUND_LED_PIN);
  pinMode(LIGHT_PIN, INPUT);
  pinMode(MOTION_PIN, INPUT);
  pinMode(TRANSMITTING_AND_BROADCASTING_LED_PIN, OUTPUT);
  digitalWrite(TRANSMITTING_AND_BROADCASTING_LED_PIN, HIGH);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(MOTION_RELAY_PIN, OUTPUT);
  pinMode(PORCH_LIGHT_RELAY_PIN, OUTPUT);
  pinMode(LIGHT_1_RELAY_PIN, OUTPUT);
  pinMode(LIGHT_2_RELAY_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(MOTION_RELAY_PIN, HIGH);
  digitalWrite(PORCH_LIGHT_RELAY_PIN, HIGH);
  digitalWrite(LIGHT_1_RELAY_PIN, HIGH);
  digitalWrite(LIGHT_2_RELAY_PIN, HIGH);
  delay(2*60*1000); // cause motion sensor
}

bool isPorchLightAutoModeOn = true;
bool isClapSwitchModeOn = true;
bool wasThiefDetected = false;
bool wasPorchLightOn = false;
bool wasLightOn = false;

unsigned long lastTransmittingToRootMillis = 0;

void loop() {
  mesh.update();

  // Update Living Room data to Root every 10s
  if (millis() - lastTransmittingToRootMillis > 10*1000) {
    firstPackage.pieceArray[0].updatePieceFrom("100", "0");
    firstPackage.pieceArray[1].updatePieceFrom("101", String(isMotion()));
    firstPackage.pieceArray[2].updatePieceFrom("102", String(hasLight()));
    firstPackage.pieceArray[3].updatePieceFrom("103", String(getPorchLightState()));
    firstPackage.pieceArray[4].updatePieceFrom("104", String(isPorchLightAutoModeOn));
    firstPackage.pieceArray[5].updatePieceFrom("105", String(getLight1State()));
    firstPackage.pieceArray[6].updatePieceFrom("106", String(getLight2State()));
    firstPackage.pieceArray[7].updatePieceFrom("107", String(isClapSwitchModeOn));
    transmitCleanFirstPackageWithNumFirstPieces(8);
    lastTransmittingToRootMillis = millis();
  }

  // Reception from Root via UART
  if (Serial.available()) {
    String msg = Serial.readStringUntil('\n');
    if (msg == SOUND_CAUSE_PRESS) {
      clapSwitch.index = 0;
    } else {
      parseMsgToConsiderToUpdateOrTransmitToRootOrBroadcast(msg);
    }
  }

  clapSwitch.listenAndHandle(clapSwitchHandle);

  if (isMotion()) {
    if (wasThiefDetected == false) {
      wasThiefDetected = true;
      digitalWrite(BUZZER_PIN, HIGH);
      digitalWrite(MOTION_RELAY_PIN, LOW);
      firstPackage.pieceArray[0].updatePieceFrom("101", "1");
      transmitCleanFirstPackageWithNumFirstPieces(1);
    }
  } else {
    if (wasThiefDetected == true) {
      wasThiefDetected = false;
      digitalWrite(BUZZER_PIN, LOW);
      digitalWrite(MOTION_RELAY_PIN, HIGH);
      firstPackage.pieceArray[0].updatePieceFrom("101", "0");
      transmitCleanFirstPackageWithNumFirstPieces(1);
    }
  }

  if (hasLight()) {
    if (wasLightOn == false) {
      wasLightOn = true;
      firstPackage.pieceArray[0].updatePieceFrom("102", "1");
      transmitCleanFirstPackageWithNumFirstPieces(1);
    }
  } else {
    if (wasLightOn == true) {
      wasLightOn = false;
      firstPackage.pieceArray[0].updatePieceFrom("102", "0");
      transmitCleanFirstPackageWithNumFirstPieces(1);
    }
  }

  if (hasLight() && isPorchLightAutoModeOn) {
    if (wasPorchLightOn == true) {
      wasPorchLightOn = false;
      digitalWrite(PORCH_LIGHT_RELAY_PIN, HIGH);
      firstPackage.pieceArray[0].updatePieceFrom("103", "0");
      transmitCleanFirstPackageWithNumFirstPieces(1);
    }
  } else if (!hasLight() && isPorchLightAutoModeOn) {
    if (wasPorchLightOn == false) {
      wasPorchLightOn = true;
      digitalWrite(PORCH_LIGHT_RELAY_PIN, LOW);
      firstPackage.pieceArray[0].updatePieceFrom("103", "1");
      transmitCleanFirstPackageWithNumFirstPieces(1);
    }
  }

  if (porchLightButton.pressed()) {
    digitalWrite(PORCH_LIGHT_RELAY_PIN, !digitalRead(PORCH_LIGHT_RELAY_PIN));
    isPorchLightAutoModeOn = false;
    wasPorchLightOn = getPorchLightState();
    firstPackage.pieceArray[0].updatePieceFrom("103", String(getPorchLightState()));
    firstPackage.pieceArray[1].updatePieceFrom("104", (String)isPorchLightAutoModeOn);
    transmitCleanFirstPackageWithNumFirstPieces(2);
  }

  if (light1Button.pressed()) {
    digitalWrite(LIGHT_1_RELAY_PIN, !digitalRead(LIGHT_1_RELAY_PIN));
    firstPackage.pieceArray[0].updatePieceFrom("105", String(getLight1State()));
    transmitCleanFirstPackageWithNumFirstPieces(1);
  }

  if (light2Button.pressed()) {
    digitalWrite(LIGHT_2_RELAY_PIN, !digitalRead(LIGHT_2_RELAY_PIN));
    firstPackage.pieceArray[0].updatePieceFrom("106", String(getLight2State()));
    transmitCleanFirstPackageWithNumFirstPieces(1);
  }
}

void clapSwitchHandle(String type) {
  if (type == "252") {
    firstPackage.pieceArray[0].updatePieceFrom("100", "1");
    transmitCleanFirstPackageWithNumFirstPieces(1);
  } else if (type == "TOGGLE_LIGHT_1") {
    if (isClapSwitchModeOn) {
      digitalWrite(LIGHT_1_RELAY_PIN, !digitalRead(LIGHT_1_RELAY_PIN));
      firstPackage.pieceArray[0].updatePieceFrom("105", String(getLight1State()));
      transmitCleanFirstPackageWithNumFirstPieces(1);
    }
  } else if (type == "TOGGLE_LIGHT_2") {
    if (isClapSwitchModeOn) {
      digitalWrite(LIGHT_2_RELAY_PIN, !digitalRead(LIGHT_2_RELAY_PIN));
      firstPackage.pieceArray[0].updatePieceFrom("106", String(getLight2State()));
      transmitCleanFirstPackageWithNumFirstPieces(1);
    }
  }
}

void messageBroadcastingScheduled() {
  firstPackage.clearAllExceptPieceArray();
  firstPackage.from = mesh.getNodeId();
  firstPackage.to = GARDEN_NODE_ID;
  firstPackage.nPieces = 1;
  firstPackage.pieceArray[0].updatePieceFrom("102", (String)hasLight());
  firstDoc.clear();
  firstPackage.nestInto(firstDoc);
  broadcast(firstDoc.as<String>());
}

void receivedBroadcastingMsgCallback(uint32_t from, String &msg) {
  parseMsgToConsiderToUpdateOrTransmitToRootOrBroadcast(msg);
}

void parseMsgToConsiderToUpdateOrTransmitToRootOrBroadcast(String msg) {
  firstDoc.clear();
  deserializeJson(firstDoc, msg);
  uint32_t toID = firstDoc["to"].as<uint32_t>();
  if (toID == mesh.getNodeId()) {
    updateLivingRoomFrom(firstDoc);
  } else if (toID == ROOT_NODE_ID) {
    transmit(msg);
  } else {
    broadcast(msg);
  }
}

void updateLivingRoomFrom(JsonDocument docParam) {
  firstPackage.clearAllExceptPieceArray();
  firstPackage.updateFrom(docParam);
  for (uint8_t i = 0; i < firstPackage.nPieces; i++) {
    uint8_t value = firstPackage.pieceArray[i].value.toInt();
    if (firstPackage.pieceArray[i].id == "103") {
      digitalWrite(PORCH_LIGHT_RELAY_PIN, !value);
      isPorchLightAutoModeOn = false;
      wasPorchLightOn = value;
      secondPackage.pieceArray[0].updatePieceFrom("104", String(isPorchLightAutoModeOn));
      transmitCleanSecondPackageWithNumFirstPieces(1);
    } else if (firstPackage.pieceArray[i].id == "104") {
      isPorchLightAutoModeOn = value;
    } else if (firstPackage.pieceArray[i].id == "105") {
      digitalWrite(LIGHT_1_RELAY_PIN, !value);
    } else if (firstPackage.pieceArray[i].id == "106") {
      digitalWrite(LIGHT_2_RELAY_PIN, !value);
    } else if (firstPackage.pieceArray[i].id == "107") {
      isClapSwitchModeOn = value;
    }
  }
}

void transmitCleanFirstPackageWithNumFirstPieces(uint8_t nPiecesParam) {
  firstPackage.clearAllExceptPieceArray();
  firstPackage.nPieces = nPiecesParam;
  firstDoc.clear();
  firstPackage.nestInto(firstDoc);
  transmit(firstDoc.as<String>());
}

void transmitCleanSecondPackageWithNumFirstPieces(uint8_t nPiecesParam) {
  secondPackage.clearAllExceptPieceArray();
  secondPackage.nPieces = nPiecesParam;
  secondDoc.clear();
  secondPackage.nestInto(secondDoc);
  transmit(secondDoc.as<String>());
}

void transmit(String msg) {
  digitalWrite(TRANSMITTING_AND_BROADCASTING_LED_PIN, !digitalRead(TRANSMITTING_AND_BROADCASTING_LED_PIN));
  Serial.println(msg);
}

void broadcast(String msg) {
  digitalWrite(TRANSMITTING_AND_BROADCASTING_LED_PIN, !digitalRead(TRANSMITTING_AND_BROADCASTING_LED_PIN));
  mesh.sendBroadcast(msg);
}

bool hasLight() {
  return !digitalRead(LIGHT_PIN);
}

bool isMotion() {
  return digitalRead(MOTION_PIN);
}

bool getPorchLightState() {
  return !digitalRead(PORCH_LIGHT_RELAY_PIN);
}

bool getLight1State() {
  return !digitalRead(LIGHT_1_RELAY_PIN);
}

bool getLight2State() {
  return !digitalRead(LIGHT_2_RELAY_PIN);
}