#include "Chip.h"

Chip::Chip(const char *name, ChipType type) {
  this->name = name;
  this->type = type;
}

const char *Chip::getName() { return this->name; }
ChipType Chip::getType() { return this->type; }
