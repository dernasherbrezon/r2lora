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

int16_t LoRaModule::init(Chip *chip) {
  log_i("initialize chip: %s", chip->getName());
  reset();
  this->module = new Module(PIN_CS, PIN_DI0, PIN_RST);
  if (isSX1278()) {
    SX1278 *sx = new SX1278(module);
    // init with default settings.
    // rx/tx will override essential
    int16_t status = sx->begin();
    if (status != ERR_NONE) {
      log_e("unable to initialize lora chip: %d", status);
      return status;
    }
    this->phys = sx;
  } else if (isSX1276()) {
    SX1276 *sx = new SX1276(module);
    int16_t status = sx->begin();
    if (status != ERR_NONE) {
      log_e("unable to initialize lora chip: %d", status);
      return status;
    }
    this->phys = sx;
  } else {
    log_e("unsupported chip found: %d", chip->getType());
    return ERR_CHIP_NOT_FOUND;
  }
  this->type = chip->getType();
  return ERR_NONE;
}

void LoRaModule::reset() {
  if (this->receivingData) {
    this->stopRx();
  }
  if (this->module != NULL) {
    free(this->module);
    this->module = NULL;
  }
  if (this->phys != NULL) {
    free(this->phys);
    this->phys = NULL;
  }
  // some default type
  this->type = ChipType::TYPE_SX1278;
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

int16_t LoRaModule::startRx(float freq, float bw, uint8_t sf, uint8_t cr, uint8_t syncWord, uint16_t preambleLength, uint8_t gain, uint8_t ldro) {
  int16_t status = this->begin(freq, bw, sf, cr, syncWord, 10, preambleLength, gain, ldro);
  if (status != ERR_NONE) {
    log_e("unable to init rx: %d", status);
    return status;
  }
  SX127x *genericSx;
  if (isSX1278()) {
    SX1278 *sx = (SX1278 *)phys;
    genericSx = sx;
  } else if (isSX1276()) {
    SX1276 *sx = (SX1276 *)phys;
    genericSx = sx;
  } else {
    return ERR_CHIP_NOT_FOUND;
  }
  genericSx->setDio0Action(setFlag);
  status = genericSx->startReceive();
  if (status == ERR_NONE) {
    this->receivingData = true;
    log_i("RX started");
    if (this->rxStartedCallback != NULL) {
      this->rxStartedCallback();
    }
  } else {
    log_e("unable to start rx: %d", status);
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
  if (isSX1278()) {
    SX1278 *sx = (SX1278 *)this->phys;
    status = sx->startReceive();
  } else if (isSX1276()) {
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
  /**
    do not correct frequency error
    doppler correction should be more sophisticated, i.e.
     - take frequency error upon reception,
     - take time of the reception and calculate expected doppler frequency error
     - the difference between expected error and actual error is the "real
  error" RadioLib however doesn't expose a function for explicit frequency
  correction
  **/
  if (isSX1278()) {
    SX1278 *sx = (SX1278 *)this->phys;
    result->setRssi(sx->getRSSI());
    result->setSnr(sx->getSNR());
    result->setFrequencyError(sx->getFrequencyError(false));
  } else if (isSX1276()) {
    SX1276 *sx = (SX1276 *)this->phys;
    result->setRssi(sx->getRSSI());
    result->setSnr(sx->getSNR());
    result->setFrequencyError(sx->getFrequencyError(false));
  }
  log_i("frame received: %d bytes RSSI: %f SNR: %f", result->getDataLength(), result->getRssi(), result->getSnr());
  return result;
}

void LoRaModule::stopRx() {
  log_i("RX stopped");
  int16_t status = ERR_NONE;
  if (isSX1278()) {
    SX1278 *sx = (SX1278 *)this->phys;
    status = sx->sleep();
  } else if (isSX1276()) {
    SX1276 *sx = (SX1276 *)this->phys;
    status = sx->sleep();
  }
  if (status != ERR_NONE) {
    log_e("unable to put module back to sleep: %d", status);
  }
  this->receivingData = false;
  if (this->rxStoppedCallback != NULL) {
    this->rxStoppedCallback();
  }
}

bool LoRaModule::isReceivingData() {
  return this->receivingData;
}

void LoRaModule::setOnRxStartedCallback(std::function<void()> func) {
  this->rxStartedCallback = func;
}
void LoRaModule::setOnRxStoppedCallback(std::function<void()> func) {
  this->rxStoppedCallback = func;
}

int LoRaModule::getTempRaw(int8_t *value) {
  if (this->phys == NULL) {
    return -1;
  }
  if (isSX1278()) {
    SX1278 *sx = (SX1278 *)this->phys;
    *value = sx->getTempRaw();
    return 0;
  } else if (isSX1276()) {
    SX1276 *sx = (SX1276 *)this->phys;
    *value = sx->getTempRaw();
    return 0;
  }
  return -1;
}

int16_t LoRaModule::tx(uint8_t *data, size_t dataLength, float freq, float bw, uint8_t sf, uint8_t cr, uint8_t syncWord, uint16_t preambleLength, int8_t power) {
  int16_t status = this->begin(freq, bw, sf, cr, syncWord, power, preambleLength, 0, 0);
  if (status != ERR_NONE) {
    log_e("unable to init tx: %d", status);
    return status;
  }
  if (isSX1278()) {
    SX1278 *sx = (SX1278 *)this->phys;
    status = sx->transmit(data, dataLength);
  } else if (isSX1276()) {
    SX1276 *sx = (SX1276 *)this->phys;
    status = sx->transmit(data, dataLength);
  } else {
    status = ERR_CHIP_NOT_FOUND;
  }
  if (status != ERR_NONE) {
    log_e("unable to tx: %d", status);
  }
  return status;
}

int16_t LoRaModule::begin(float freq, float bw, uint8_t sf, uint8_t cr, uint8_t syncWord, int8_t power, uint16_t preambleLength, uint8_t gain, uint8_t ldro) {
  if (isSX1278()) {
    SX1278 *sx = (SX1278 *)this->phys;

    int16_t status = sx->setFrequency(freq);
    RADIOLIB_ASSERT(status);

    status = sx->setSyncWord(syncWord);
    RADIOLIB_ASSERT(status);

    status = sx->setPreambleLength(preambleLength);
    RADIOLIB_ASSERT(status);

    status = sx->setBandwidth(bw);
    RADIOLIB_ASSERT(status);

    status = sx->setSpreadingFactor(sf);
    RADIOLIB_ASSERT(status);

    status = sx->setCodingRate(cr);
    RADIOLIB_ASSERT(status);

    status = sx->setOutputPower(power);
    RADIOLIB_ASSERT(status);

    status = sx->setGain(gain);
    RADIOLIB_ASSERT(status);

    switch (ldro) {
      case LDRO_AUTO:
        status = sx->autoLDRO();
        break;
      case LDRO_ON:
        status = sx->forceLDRO(true);
        break;
      case LDRO_OFF:
        status = sx->forceLDRO(false);
        break;
      default:
        break;
    }
    RADIOLIB_ASSERT(status);

    status = sx->setCRC(true);
    RADIOLIB_ASSERT(status);
    return ERR_NONE;

  } else if (isSX1276()) {
    SX1276 *sx = (SX1276 *)phys;

    int16_t status = sx->setFrequency(freq);
    RADIOLIB_ASSERT(status);

    status = sx->setSyncWord(syncWord);
    RADIOLIB_ASSERT(status);

    status = sx->setPreambleLength(preambleLength);
    RADIOLIB_ASSERT(status);

    status = sx->setBandwidth(bw);
    RADIOLIB_ASSERT(status);

    status = sx->setSpreadingFactor(sf);
    RADIOLIB_ASSERT(status);

    status = sx->setCodingRate(cr);
    RADIOLIB_ASSERT(status);

    status = sx->setOutputPower(power);
    RADIOLIB_ASSERT(status);

    status = sx->setGain(gain);
    RADIOLIB_ASSERT(status);

    switch (ldro) {
      case LDRO_AUTO:
        status = sx->autoLDRO();
        break;
      case LDRO_ON:
        status = sx->forceLDRO(true);
        break;
      case LDRO_OFF:
        status = sx->forceLDRO(false);
        break;
      default:
        break;
    }
    RADIOLIB_ASSERT(status);

    status = sx->setCRC(true);
    RADIOLIB_ASSERT(status);
    return ERR_NONE;
  } else {
    return ERR_UNKNOWN;
  }
}

bool LoRaModule::isSX1278() {
  if (this->type == ChipType::TYPE_SX1278) {
    return true;
  }
  return false;
}

bool LoRaModule::isSX1276() {
  if (this->type == ChipType::TYPE_SX1276_868 || this->type == ChipType::TYPE_SX1276_433 || this->type == ChipType::TYPE_SX1276) {
    return true;
  }
  return false;
}

LoRaModule::~LoRaModule() {
  reset();
}