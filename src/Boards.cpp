#include "Boards.h"

Boards::Boards() {
  this->boards.push_back(Board("TTGO LORA32 V1.0", 18, 26, 14, 5, 19, 27));
  this->boards.push_back(Board("TTGO LORA32 V2.0", 18, 26, 12, 5, 19, 27));

  this->board_names = (char *)malloc(sizeof(char) * STRING_LEN * this->boards.size());
  this->board_indices = (char *)malloc(sizeof(char) * STRING_LEN * this->boards.size());

  for (size_t i = 0; i < this->boards.size(); i++) {
    size_t original_len = strnlen(this->boards[i].getName(), STRING_LEN);
    strncpy(this->board_names + i * STRING_LEN, this->boards[i].getName(), original_len + 1);
    snprintf(this->board_indices + i * STRING_LEN, STRING_LEN, "%d", i);
  }
}

char *Boards::getBoardIndices() { return this->board_indices; }

char *Boards::getBoardNames() { return this->board_names; }

std::vector<Board> Boards::getAll() { return this->boards; }