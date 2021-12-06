#include "Chips.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRING_LEN 256

Chips::Chips() {
  this->chips.push_back(Chip("SX1278 - 433Mhz", ChipType::TYPE_SX1278));
  this->chips.push_back(Chip("SX1276 - 868/915Mhz", ChipType::TYPE_SX1276));

  this->chip_names =
      (char *)malloc(sizeof(char) * STRING_LEN * this->chips.size());
  this->chip_indices =
      (char *)malloc(sizeof(char) * STRING_LEN * this->chips.size());

  for (size_t i = 0; i < this->chips.size(); i++) {
    size_t original_len = strnlen(this->chips[i].getName(), STRING_LEN);
    strncpy(this->chip_names + i * STRING_LEN, this->chips[i].getName(),
            original_len + 1);
    snprintf(this->chip_indices + i * STRING_LEN, STRING_LEN, "%d", i);
  }
}

char *Chips::getChipNames() { return this->chip_names; }
char *Chips::getChipIndices() { return this->chip_indices; }
std::vector<Chip> Chips::getAll() { return this->chips; }