#ifndef LoRaModule_h
#define LoRaModule_h

#include <RadioLib.h>
#include "Chip.h"
#include "LoRaFrame.h"
#include "ObservationRequest.h"

class LoRaModule {
 public:
  ~LoRaModule();
  int setup(Chip chip);
  int begin(ObservationRequest *req);
  LoRaFrame *loop();
  void end();
  bool isReceivingData();

 private:
  Module *module;
  PhysicalLayer *phys;
  ChipType type;
  LoRaFrame *readFrame();
  bool receivingData = false;
};

#endif