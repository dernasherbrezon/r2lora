#ifndef LoRaModule_h
#define LoRaModule_h

#include <RadioLib.h>
#include <stdint.h>

#include "Chip.h"
#include "LoRaFrame.h"

enum LDRO_TYPE { LDRO_AUTO = 0, LDRO_ON = 1, LDRO_OFF = 2 };

class LoRaModule {
 public:
  ~LoRaModule();
  int init(Chip chip);
  virtual int startRx(float freq, float bw, uint8_t sf, uint8_t cr, uint8_t syncWord, uint16_t preambleLength, uint8_t gain, uint8_t ldro);
  virtual LoRaFrame *loop();
  virtual void stopRx();
  virtual bool isReceivingData();
  virtual int tx(uint8_t *data, size_t dataLength, int8_t power);
  int getTempRaw(int8_t *value);

 private:
  Module *module = NULL;
  PhysicalLayer *phys = NULL;
  ChipType type = ChipType::TYPE_SX1278;  // some default chip type
  bool receivingData = false;

  LoRaFrame *readFrame();
  void reset();
};

#endif