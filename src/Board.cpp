#include "Board.h"

Board::Board(const char *name, RADIOLIB_PIN_TYPE cs, RADIOLIB_PIN_TYPE irq,
             RADIOLIB_PIN_TYPE rst, int8_t sck, int8_t miso, int8_t mosi) {
  this->name = name;
  this->cs = cs;
  this->irq = irq;
  this->rst = rst;
  this->sck = sck;
  this->miso = miso;
  this->mosi = mosi;
}

const char *Board::getName() { return this->name; }
RADIOLIB_PIN_TYPE Board::getCs() { return this->cs; }
RADIOLIB_PIN_TYPE Board::getIrq() { return this->irq; }
RADIOLIB_PIN_TYPE Board::getRst() { return this->rst; }
int8_t Board::getSck() { return this->sck; }
int8_t Board::getMiso() { return this->miso; }
int8_t Board::getMosi() { return this->mosi; } 