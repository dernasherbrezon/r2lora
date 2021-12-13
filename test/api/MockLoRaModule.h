#ifndef MockLoRaModule_h
#define MockLoRaModule_h

#include <LoRaModule.h>

class MockLoRaModule : public LoRaModule {
 public:
  int begin(ObservationRequest *req);
  LoRaFrame *loop();
  void end();
  bool isReceivingData();
  int tx(uint8_t *data, size_t dataLength, int8_t power);

  int beginCode = 0;
  std::vector<LoRaFrame *> expectedFrames;
  bool receiving = false;
  int txCode = 0;

 private:
  int currentFrameIndex = 0;
};

#endif
