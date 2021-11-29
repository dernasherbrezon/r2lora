#ifndef Board_h
#define Board_h

#include <RadioLib.h>

class Board {
 public:
  Board(const char *name, RADIOLIB_PIN_TYPE cs, RADIOLIB_PIN_TYPE irq,
        RADIOLIB_PIN_TYPE rst, int8_t sck, int8_t miso, int8_t mosi);

  const char *getName();

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
