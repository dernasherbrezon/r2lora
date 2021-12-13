#include "MockLoRaModule.h"

int MockLoRaModule::begin(ObservationRequest *req) { return beginCode; }

LoRaFrame *MockLoRaModule::loop() {
  if (expectedFrames.empty() || currentFrameIndex >= expectedFrames.size()) {
    return NULL;
  }
  LoRaFrame *result = expectedFrames[currentFrameIndex];
  currentFrameIndex++;
  return result;
}

void end() {
  // do nothing
}

bool MockLoRaModule::isReceivingData() { return receiving; }
int MockLoRaModule::tx(uint8_t *data, size_t dataLength, int8_t power) {
  return txCode;
}
