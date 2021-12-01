#ifndef Board_h
#define Board_h

#include <RadioLib.h>

class Board {
 public:
  Board(const char *name, RADIOLIB_PIN_TYPE cs, RADIOLIB_PIN_TYPE irq,
        RADIOLIB_PIN_TYPE rst, int8_t sck, int8_t miso, int8_t mosi);

  const char *getName();
  RADIOLIB_PIN_TYPE getCs();
  RADIOLIB_PIN_TYPE getIrq();
  RADIOLIB_PIN_TYPE getRst();
  int8_t getSck();
  int8_t getMiso();
  int8_t getMosi();

 private:
  const char *name;
  RADIOLIB_PIN_TYPE cs;
  RADIOLIB_PIN_TYPE irq;
  RADIOLIB_PIN_TYPE rst;
  int8_t sck;
  int8_t miso;
  int8_t mosi;
};

#endif
