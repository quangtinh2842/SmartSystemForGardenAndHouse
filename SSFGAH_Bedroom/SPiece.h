#include <ArduinoJson.h>

struct SPiece {
  String id;
  String value;

  void updatePieceFrom(String idParam, String valueParam) {
    this->id = idParam;
    this->value = valueParam;
  }

  void nestIntoPackagePiecesFrom(JsonDocument &doc, uint8_t indexParam) {
    doc["pieceArray"][indexParam]["id"] = this->id;
    doc["pieceArray"][indexParam]["value"] = this->value;
  }

  void updateFromPackagePieces(JsonDocument doc, uint8_t indexParam) {
    this->id = doc["pieceArray"][indexParam]["id"].as<String>();
    this->value = doc["pieceArray"][indexParam]["value"].as<String>();
  }
};