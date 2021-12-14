#ifndef MockLoRaModule_h
#define MockLoRaModule_h

#include <LoRaModule.h>

class MockLoRaModule : public LoRaModule {
 public:
  int begin(float freq, float bw, uint8_t sf, uint8_t cr, uint8_t syncWord, uint16_t preambleLength, uint8_t gain, uint8_t ldro);
  LoRaFrame *loop();
  void end();
  bool isReceivingData();
  int tx(uint8_t *data, size_t dataLength, int8_t power);

  int beginCode = 0;
  std::vector<LoRaFrame *> expectedFrames;
  bool receiving = false;
  int txCode = 0;
  int currentFrameIndex = 0;

};

#endif
