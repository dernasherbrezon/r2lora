#ifndef ObservationRequest_h
#define ObservationRequest_h

#include <WString.h>
#include <stdint.h>

enum LDRO_TYPE { LDRO_AUTO = 0, LDRO_ON = 1, LDRO_OFF = 2 };

class ObservationRequest {
 public:
  ObservationRequest();
  int parseJson(String input);
  float getFreq();
  void setFreq(float freq);
  float getBw();
  void setBw(float bw);
  uint8_t getSf();
  void setSf(uint8_t sf);
  uint8_t getCr();
  void setCr(uint8_t cr);
  uint8_t getSyncWord();
  void setSyncWord(uint8_t syncWord);
  int8_t getPower();
  void setPower(int8_t power);
  uint16_t getPreambleLength();
  void setPreambleLength(uint16_t preambleLength);
  uint8_t getGain();
  void setGain(uint8_t gain);
  uint8_t getLdro();
  void setLdro(uint8_t ldro);

 private:
  float freq = 0.0F;            // = (434.0F)
  float bw = 0.0F;              // = (125.0F)
  uint8_t sf = 0;               // = (uint8_t)9U
  uint8_t cr = 0;               // = (uint8_t)7U
  uint8_t syncWord = 0;         // = (uint8_t)18U
  int8_t power = 0;             // = (int8_t)10
  uint16_t preambleLength = 0;  // = (uint16_t)8U
  uint8_t gain = 0;             // = (uint8_t)0U
  uint8_t ldro = 0;             // 0 - auto, 1 - enable, 2 - disable
};

#endif
