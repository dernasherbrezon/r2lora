#include "LoRaModule.h"

#include <RadioLib.h>
#include <esp32-hal-log.h>
#include <time.h>

// defined in platformio.ini
#ifndef PIN_CS
#define PIN_CS 0
#endif
#ifndef PIN_DI0
#define PIN_DI0 0
#endif
#ifndef PIN_RST
#define PIN_RST 0
#endif

// flag to indicate that a packet was received
volatile bool receivedFlag = false;

// disable interrupt when it's not needed
volatile bool enableInterrupt = true;

int LoRaModule::setup(Chip chip) {
  log_i("init module");
  this->module = new Module(PIN_CS, PIN_DI0, PIN_RST);
  if (chip.getType() == ChipType::TYPE_SX1278) {
    this->phys = new SX1278(module);
  } else if (chip.getType() == ChipType::TYPE_SX1276) {
    this->phys = new SX1276(module);
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
  } else if (this->type == ChipType::TYPE_SX1276) {
    SX1276 *sx = (SX1276 *)phys;
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
    log_e("unable to begin LoRa: %d", status);
    return status;
  }
  genericSx->setDio0Action(setFlag);
  status = genericSx->startReceive();
  if (status == ERR_NONE) {
    this->receivingData = true;
  }
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
  } else if (this->type == ChipType::TYPE_SX1276) {
    SX1276 *sx = (SX1276 *)this->phys;
    status = sx->startReceive();
  }
  if (status != ERR_NONE) {
    log_e("unable to put module back to receive mode: %d", status);
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
    free(result);
    return NULL;
  }
  result->setData(data);

  int16_t status = this->phys->readData(data, result->getDataLength());
  if (status != ERR_NONE) {
    delete result;
    log_e("unable to read the frame: %d", status);
    return NULL;
  }
  time_t now;
  time(&now);
  result->setTimestamp(now);
  if (this->type == ChipType::TYPE_SX1278) {
    SX1278 *sx = (SX1278 *)this->phys;
    result->setRssi(sx->getRSSI());
    result->setSnr(sx->getSNR());
    result->setFrequencyError(sx->getFrequencyError());
  } else if (this->type == ChipType::TYPE_SX1276) {
    SX1276 *sx = (SX1276 *)this->phys;
    result->setRssi(sx->getRSSI());
    result->setSnr(sx->getSNR());
    result->setFrequencyError(sx->getFrequencyError());
  }
  log_i("frame received: %d bytes RSSI: %f SNR: %f", result->getDataLength(),
        result->getRssi(), result->getSnr());
  return result;
}

void LoRaModule::end() {
  int16_t status = ERR_NONE;
  if (this->type == ChipType::TYPE_SX1278) {
    SX1278 *sx = (SX1278 *)this->phys;
    status = sx->sleep();
  } else if (this->type == ChipType::TYPE_SX1276) {
    SX1276 *sx = (SX1276 *)this->phys;
    status = sx->sleep();
  }
  if (status != ERR_NONE) {
    log_e("unable to put module back to sleep: %d", status);
  }
  this->receivingData = false;
}

bool LoRaModule::isReceivingData() { return this->receivingData; }

LoRaModule::~LoRaModule() {
  if (this->phys != NULL) {
    // no private destructor for all RadioLib modules
    free(this->phys);
  }
  if (this->module != NULL) {
    free(this->module);
  }
}