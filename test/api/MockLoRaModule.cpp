#include "MockLoRaModule.h"

int16_t MockLoRaModule::startRx(float freq, float bw, uint8_t sf, uint8_t cr,
                          uint8_t syncWord, uint16_t preambleLength,
                          uint8_t gain, uint8_t ldro) {
  return rxCode;
}

LoRaFrame *MockLoRaModule::loop() {
  if (expectedFrames.empty() || currentFrameIndex >= expectedFrames.size()) {
    return NULL;
  }
  LoRaFrame *result = expectedFrames[currentFrameIndex];
  currentFrameIndex++;
  return result;
}

void MockLoRaModule::stopRx() {
  // do nothing
}

bool MockLoRaModule::isReceivingData() {
  return receiving;
}
int16_t MockLoRaModule::tx(uint8_t *data, size_t dataLength, float freq, float bw, uint8_t sf, uint8_t cr, uint8_t syncWord, uint16_t preambleLength, int8_t power) {
  return txCode;
}
