#include "LoRaModule.h"
#include <RadioLib.h>
#include <esp32-hal-spi.h>

void LoRaModule::setup(Board board) {
    SPISettings spiSettings(2000000, MSBFIRST, SPI_MODE0);
    SPIClass spi(VSPI);
    spi.begin(board.getSck(), board.getMiso(), board.getMosi(), board.getCs());
    Module *module = new Module(board.getCs(), board.getIrq(), board.getRst(), spi, spiSettings);
    SX1278 *sx = new SX1278(module);
}

int LoRaModule::begin(ObservationRequest *req) {
    return -1;
}

LoRaFrame *LoRaModule::loop() {
    return NULL;
}