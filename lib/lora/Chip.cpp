#include "Chip.h"

Chip::Chip(const char *name, ChipType type) {
  this->name = name;
  this->type = type;
  switch (type) {
    case TYPE_SX1278:
      minLoraFrequency = 137.0;
      maxLoraFrequency = 525.0;
      loraSupported = true;

      minFskFrequency = 137.0;
      maxFskFrequency = 525.0;
      fskSupported = true;
      break;
    case TYPE_SX1276:
      minLoraFrequency = 137.0;
      maxLoraFrequency = 1020.0;
      loraSupported = true;

      minFskFrequency = 137.0;
      maxFskFrequency = 1020.0;
      fskSupported = true;
      break;
    default:
      break;
  }
}

const char *Chip::getName() {
  return this->name;
}
ChipType Chip::getType() {
  return this->type;
}
