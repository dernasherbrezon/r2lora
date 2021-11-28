
#include "Configurator.h"

static const char *LOG_TAG = "r2lora";
static const char CHIP_TYPE_NAMES[][STRING_LEN] = {
    "LLCC68", "RFM95",  "RM96",   "RFM97",  "RFM98",  "SX1261",
    "SX1262", "SX1268", "SX1272", "SX1273", "SX1276", "SX1277",
    "SX1278", "SX1279", "SX1280", "SX1281", "SX1282"};

Configurator::Configurator() {
  this->dnsServer = new DNSServer();
  this->webServer = new WebServer(80);
  this->conf = new IotWebConf(LOG_TAG, dnsServer, webServer, "", "0.1");

  this->boardType = new IotWebConfSelectParameter(
      "LoRa chip", "chipType", this->chipType, STRING_LEN,
      (char *)CHIP_TYPE_NAMES, (char *)CHIP_TYPE_NAMES,
      sizeof(CHIP_TYPE_NAMES) / STRING_LEN, STRING_LEN);

  this->allCustomParameters =
      new IotWebConfParameterGroup("allCustomParameters", "");
  allCustomParameters->addItem(this->boardType);
  this->conf->addParameterGroup(this->allCustomParameters);

  this->conf->init();

  this->webServer->on("/", [this] { this->conf->handleConfig(); });
  this->webServer->onNotFound([this]() { this->conf->handleNotFound(); });
}

void Configurator::loop() {
    if( this->conf == NULL ) {
        return;
    }
    this->conf->doLoop();
}

Configurator::~Configurator() {
  if (this->boardType != NULL) {
    free(this->boardType);
  }
  if (this->allCustomParameters != NULL) {
    free(this->allCustomParameters);
  }
  if (this->dnsServer != NULL) {
    this->dnsServer->stop();
    free(this->dnsServer);
  }
  if (this->webServer != NULL) {
    this->webServer->stop();
    free(this->webServer);
  }
  if (this->conf != NULL) {
    free(this->conf);
  }
}
