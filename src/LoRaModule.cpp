#include "LoRaModule.h"

#include <RadioLib.h>
#include <esp32-hal-log.h>
#include <esp32-hal-spi.h>

int LoRaModule::setup(Board board, Chip chip) {
  SPISettings spiSettings(2000000, MSBFIRST, SPI_MODE0);
  SPIClass spi(VSPI);
  spi.begin(board.getSck(), board.getMiso(), board.getMosi(), board.getCs());
  Module *module = new Module(board.getCs(), board.getIrq(), board.getRst(),
                              spi, spiSettings);
  if (chip.getType() == ChipType::TYPE_SX1278) {
    this->phys = new SX1278(module);
  } else if (chip.getType() == ChipType::TYPE_SX1273) {
    this->phys = new SX1273(module);
  } else {
    log_e("unknown chip type: %d", chip.getType());
    return -1;
  }
  this->type = chip.getType();
  return 0;
}

int LoRaModule::begin(ObservationRequest *req) {
  if (this->type == ChipType::TYPE_SX1278) {
    SX1278 *sx = (SX1278 *)phys;
    // FIXME
    // sx->begin();
  } else if (this->type == ChipType::TYPE_SX1273) {
    SX1273 *sx = (SX1273 *)phys;
    //   sx->begin();
  }
  return -1;
}

LoRaFrame *LoRaModule::loop() { return NULL; }