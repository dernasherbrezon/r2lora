#include <stdint.h>

class ObservationRequest {
 private:
  float freq;               // = (434.0F)
  float bw;                 // = (125.0F)
  uint8_t sf;               // = (uint8_t)9U
  uint8_t cr;               // = (uint8_t)7U
  uint8_t syncWord;         // = (uint8_t)18U
  int8_t power;             // = (int8_t)10
  uint16_t preambleLength;  // = (uint16_t)8U
  uint8_t gain;             // = (uint8_t)0U
  uint8_t ldro;             // 0 - auto, 1 - enable, 2 - disable
  bool crc;
};
