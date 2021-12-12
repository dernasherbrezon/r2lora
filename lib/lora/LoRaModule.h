#ifndef LoRaModule_h
#define LoRaModule_h

#include <RadioLib.h>
#include <stdint.h>

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
  int getTempRaw(int8_t *value);
  int tx(uint8_t *data, size_t dataLength, int8_t power);

 private:
  Module *module = NULL;
  PhysicalLayer *phys = NULL;
  ChipType type = ChipType::TYPE_SX1278;  // some default chip type
  bool receivingData = false;

  LoRaFrame *readFrame();
};

#endif