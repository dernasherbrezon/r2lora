#ifndef LoRaModule_h
#define LoRaModule_h

#include "Board.h"
#include "Chip.h"
#include "LoRaFrame.h"
#include "ObservationRequest.h"

class LoRaModule {
 public:
  int setup(Board board, Chip chip);
  int begin(ObservationRequest *req);
  LoRaFrame *loop();

 private:
  PhysicalLayer *phys;
  ChipType type;
  LoRaFrame *readFrame();
};

#endif