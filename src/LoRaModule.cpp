#include "LoRaModule.h"

#include <RadioLib.h>
#include <esp32-hal-log.h>
#include <esp32-hal-spi.h>

// flag to indicate that a packet was received
volatile bool receivedFlag = false;

// disable interrupt when it's not needed
volatile bool enableInterrupt = true;

int LoRaModule::setup(Board board, Chip chip) {
  SPISettings spiSettings(2000000, MSBFIRST, SPI_MODE0);
  SPIClass spi(VSPI);
  spi.begin(board.getSck(), board.getMiso(), board.getMosi(), board.getCs());
  this->module = new Module(board.getCs(), board.getIrq(), board.getRst(), spi,
                            spiSettings);
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

// this function is called when a complete packet
// is received by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!
void setFlag(void) {
  // check if the interrupt is enabled
  if (!enableInterrupt) {
    return;
  }

  // we got a packet, set the flag
  receivedFlag = true;
}

int LoRaModule::begin(ObservationRequest *req) {
  int16_t status;
  SX127x *genericSx;
  if (this->type == ChipType::TYPE_SX1278) {
    SX1278 *sx = (SX1278 *)phys;
    status = sx->begin(req->getFreq(), req->getBw(), req->getSf(), req->getCr(),
                       req->getSyncWord(), req->getPower(),
                       req->getPreambleLength(), req->getGain());
    switch (req->getLdro()) {
      case LDRO_AUTO:
        sx->autoLDRO();
        break;
      case LDRO_ON:
        sx->forceLDRO(true);
        break;
      case LDRO_OFF:
        sx->forceLDRO(false);
        break;
      default:
        break;
    }
    // always checksum
    sx->setCRC(true);
    genericSx = sx;
  } else if (this->type == ChipType::TYPE_SX1273) {
    SX1273 *sx = (SX1273 *)phys;
    status = sx->begin(req->getFreq(), req->getBw(), req->getSf(), req->getCr(),
                       req->getSyncWord(), req->getPower(),
                       req->getPreambleLength(), req->getGain());
    switch (req->getLdro()) {
      case LDRO_AUTO:
        sx->autoLDRO();
        break;
      case LDRO_ON:
        sx->forceLDRO(true);
        break;
      case LDRO_OFF:
        sx->forceLDRO(false);
        break;
      default:
        break;
    }
    // always checksum
    sx->setCRC(true);
    genericSx = sx;
  } else {
    status = ERR_UNKNOWN;
  }
  if (status != ERR_NONE) {
    return status;
  }
  genericSx->setDio0Action(setFlag);
  status = genericSx->startReceive();
  return status;
}

LoRaFrame *LoRaModule::loop() {
  if (!receivedFlag) {
    return NULL;
  }
  enableInterrupt = false;
  receivedFlag = false;

  LoRaFrame *result = readFrame();

  // put module back to listen mode
  int16_t status = ERR_NONE;
  if (this->type == ChipType::TYPE_SX1278) {
    SX1278 *sx = (SX1278 *)this->phys;
    status = sx->startReceive();
  } else if (this->type == ChipType::TYPE_SX1273) {
    SX1273 *sx = (SX1273 *)this->phys;
    status = sx->startReceive();
  }
  if (status != ERR_NONE) {
    log_e("unable to put module back to receive: %d", status);
  }

  // we're ready to receive more packets,
  // enable interrupt service routine
  enableInterrupt = true;
  return result;
}

LoRaFrame *LoRaModule::readFrame() {
  LoRaFrame *result = new LoRaFrame();
  result->setDataLength(this->phys->getPacketLength());
  uint8_t *data = (uint8_t *)malloc(sizeof(uint8_t) * result->getDataLength());
  if (data == NULL) {
    delete result;
    return NULL;
  }
  result->setData(data);

  int16_t status = this->phys->readData(data, result->getDataLength());
  if (status != ERR_NONE) {
    delete result;
    log_e("unable to read the frame: %d", status);
    return NULL;
  }
  if (this->type == ChipType::TYPE_SX1278) {
    SX1278 *sx = (SX1278 *)this->phys;
    result->setRssi(sx->getRSSI());
    result->setSnr(sx->getSNR());
  } else if (this->type == ChipType::TYPE_SX1273) {
    SX1273 *sx = (SX1273 *)this->phys;
    result->setRssi(sx->getRSSI());
    result->setSnr(sx->getSNR());
  }
  return result;
}

void LoRaModule::end() {
  if (this->type == ChipType::TYPE_SX1278) {
    SX1278 *sx = (SX1278 *)this->phys;
    sx->sleep();
  } else if (this->type == ChipType::TYPE_SX1273) {
    SX1273 *sx = (SX1273 *)this->phys;
    sx->sleep();
  }
}

LoRaModule::~LoRaModule() {
  if (this->phys != NULL) {
    delete this->phys;
  }
  if (this->module != NULL) {
    delete this->module;
  }
}