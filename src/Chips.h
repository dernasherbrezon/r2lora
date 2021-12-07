#ifndef Chips_h
#define Chips_h

#include <vector>

#include "Chip.h"

class Chips {
 public:
  Chips();
  std::vector<Chip> getAll();
  char *getChipNames();
  char *getChipIndices();

 private:
  std::vector<Chip> chips;
  char *chip_names = NULL;
  char *chip_indices = NULL;
};

#endif
