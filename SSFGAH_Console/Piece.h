#define SENSOR_TYPE "SENSOR_TYPE"
#define CONTROL_TYPE "CONTROL_TYPE"
#define EVENT_TYPE "EVENT_TYPE"

#define INT_TYPE "INT_TYPE"
#define BOOL_TYPE "BOOL_TYPE"

#include <ArduinoJson.h>

struct Piece {
  String id;
  String name;
  String sign;
  String type; // SENSOR TYPE/ CONTROL TYPE/ EVENT_TYPE
  String valueType; // INT_TYPE/ BOOL_TYPE
  String value;
  String fromModule;
  String eventCode;

  void nestIntoPackagePiecesFrom(JsonDocument &doc, uint8_t indexParam) {
    doc["pieceArray"][indexParam]["id"] = this->id;
    // doc["pieceArray"][indexParam]["name"] = this->name;
    // doc["pieceArray"][indexParam]["sign"] = this->sign;
    // doc["pieceArray"][indexParam]["type"] = this->type;
    // doc["pieceArray"][indexParam]["valueType"] = this->valueType;
    doc["pieceArray"][indexParam]["value"] = this->value;
    // doc["pieceArray"][indexParam]["fromModule"] = this->fromModule;
    // doc["pieceArray"][indexParam]["eventCode"] = this->eventCode;
  }

  void updateFromPackagePieces(JsonDocument doc, uint8_t indexParam) {
    this->id = doc["pieceArray"][indexParam]["id"].as<String>();
    // this->name = doc["pieceArray"][indexParam]["name"].as<String>();
    // this->sign = doc["pieceArray"][indexParam]["sign"].as<String>();
    // this->type = doc["pieceArray"][indexParam]["type"].as<String>();
    // this->valueType = doc["pieceArray"][indexParam]["valueType"].as<String>();
    this->value = doc["pieceArray"][indexParam]["value"].as<String>();
    // this->fromModule = doc["pieceArray"][indexParam]["fromModule"].as<String>();
    // this->eventCode = doc["pieceArray"][indexParam]["eventCode"].as<String>();
  }
};