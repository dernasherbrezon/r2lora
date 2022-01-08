#ifndef MockLoRaModule_h
#define MockLoRaModule_h

#include <LoRaModule.h>

class MockLoRaModule : public LoRaModule {
 public:
  int16_t startRx(float freq, float bw, uint8_t sf, uint8_t cr, uint8_t syncWord, uint16_t preambleLength, uint8_t gain, uint8_t ldro);
  LoRaFrame *loop();
  void stopRx();
  bool isReceivingData();
  int16_t tx(uint8_t *data, size_t dataLength, float freq, float bw, uint8_t sf, uint8_t cr, uint8_t syncWord, uint16_t preambleLength, int8_t power);

  int16_t rxCode = ERR_NONE;
  std::vector<LoRaFrame *> expectedFrames;
  bool receiving = false;
  int16_t txCode = ERR_NONE;
  int currentFrameIndex = 0;

};

#endif
