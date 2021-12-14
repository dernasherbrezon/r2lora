#include "MockLoRaModule.h"

int MockLoRaModule::begin(float freq, float bw, uint8_t sf, uint8_t cr,
                          uint8_t syncWord, uint16_t preambleLength,
                          uint8_t gain, uint8_t ldro) {
  return beginCode;
}

LoRaFrame *MockLoRaModule::loop() {
  if (expectedFrames.empty() || currentFrameIndex >= expectedFrames.size()) {
    return NULL;
  }
  LoRaFrame *result = expectedFrames[currentFrameIndex];
  currentFrameIndex++;
  return result;
}

void MockLoRaModule::end() {
  // do nothing
}

bool MockLoRaModule::isReceivingData() { return receiving; }
int MockLoRaModule::tx(uint8_t *data, size_t dataLength, int8_t power) {
  return txCode;
}
