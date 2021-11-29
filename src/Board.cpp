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
