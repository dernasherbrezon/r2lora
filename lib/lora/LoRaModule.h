#ifndef LoRaModule_h
#define LoRaModule_h

#include <RadioLib.h>
#include <stdint.h>
#include <functional>

#include "Chip.h"
#include "LoRaFrame.h"

enum LDRO_TYPE { LDRO_AUTO = 0, LDRO_ON = 1, LDRO_OFF = 2 };

class LoRaModule {
 public:
  ~LoRaModule();
  int16_t init(Chip *chip);
  // virtual functions used by Mock overrides in tests
  virtual int16_t startRx(float freq, float bw, uint8_t sf, uint8_t cr, uint8_t syncWord, uint16_t preambleLength, uint8_t gain, uint8_t ldro);
  virtual LoRaFrame *loop();
  virtual void stopRx();
  virtual bool isReceivingData();
  virtual int16_t tx(uint8_t *data, size_t dataLength, float freq, float bw, uint8_t sf, uint8_t cr, uint8_t syncWord, uint16_t preambleLength, int8_t power);
  int getTempRaw(int8_t *value);
  void setOnRxStartedCallback(std::function<void()> func);
  void setOnRxStoppedCallback(std::function<void()> func);

 private:
  Module *module = NULL;
  PhysicalLayer *phys = NULL;
  ChipType type = ChipType::TYPE_SX1278;  // some default chip type
  bool receivingData = false;
  std::function<void()> rxStartedCallback = NULL;
  std::function<void()> rxStoppedCallback = NULL;

  LoRaFrame *readFrame();
  void reset();
  int16_t begin(float freq, float bw, uint8_t sf, uint8_t cr, uint8_t syncWord, int8_t power, uint16_t preambleLength, uint8_t gain, uint8_t ldro);
  bool isSX1278();
  bool isSX1276();
};

#endif