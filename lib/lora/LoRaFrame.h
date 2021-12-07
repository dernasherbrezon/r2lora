#ifndef LoRaFrame_h
#define LoRaFrame_h

#include <stddef.h>
#include <stdint.h>

class LoRaFrame {
 public:
  LoRaFrame();
  ~LoRaFrame();
  uint8_t *getData();
  void setData(uint8_t *data);
  size_t getDataLength();
  void setDataLength(size_t dataLength);
  float getRssi();
  void setRssi(float rssi);
  float getSnr();
  void setSnr(float snr);

 private:
  uint8_t *data = NULL;
  size_t dataLength = 0;
  float rssi = 0.0F;
  float snr = 0.0F;
};

#endif
