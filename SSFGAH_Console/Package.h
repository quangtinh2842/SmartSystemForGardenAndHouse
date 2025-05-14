#include "Piece.h"

struct Package {
  uint32_t from = NULL;
  uint32_t to = NULL;
  uint8_t nPieces;
  Piece pieceArray[20];

  void clearAllExceptPieceArray() {
    this->from = NULL;
    this->to = NULL;
    this->nPieces = 0;
    // this->pieceArray;
  }
  
  void nestInto(JsonDocument &doc) {
    if (this->from != NULL) {
      doc["from"] = this->from;
    }

    if (this->to != NULL) {
      doc["to"] = this->to;
    }

    doc["nPieces"] = this->nPieces;
    for(uint8_t i = 0; i < this->nPieces; i++) {
      pieceArray[i].nestIntoPackagePiecesFrom(doc, i);
    }
  }

  void updateFrom(JsonDocument doc) {
    this->from = doc["from"].as<uint32_t>();
    this->to = doc["to"].as<uint32_t>();
    this->nPieces = doc["nPieces"].as<uint8_t>();
    for(uint8_t i = 0; i < this->nPieces; i++) {
      pieceArray[i].updateFromPackagePieces(doc, i);
    }
  }
};