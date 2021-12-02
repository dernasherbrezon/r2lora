
#include "Configurator.h"

static const char *LOG_TAG = "r2lora";

Configurator::Configurator(WebServer *webServer) {
  this->chips = new Chips();
  this->dnsServer = new DNSServer();
  this->conf = new IotWebConf(LOG_TAG, dnsServer, webServer, "", "0.1");

  this->chipType = new IotWebConfSelectParameter(
      "Chip type", "chipType", this->chipIndex, STRING_LEN,
      this->chips->getChipIndices(), this->chips->getChipNames(),
      this->chips->getAll().size(), STRING_LEN);

  this->allCustomParameters =
      new IotWebConfParameterGroup("allCustomParameters", "r2lora");
  allCustomParameters->addItem(this->chipType);

  this->conf->addParameterGroup(this->allCustomParameters);

  this->configured = this->conf->init();
  webServer->on("/", [this] { this->conf->handleConfig(); });
  webServer->onNotFound([this]() { this->conf->handleNotFound(); });
}

void Configurator::setOnConfiguredCallback(std::function<void()> func) {
  if (this->configured) {
    func();
  }
  this->conf->setConfigSavedCallback([this, func] {
    this->configured = true;
    func();
  });
}

void Configurator::loop() {
  if (this->conf == NULL) {
    return;
  }
  this->conf->doLoop();
}

iotwebconf::NetworkState Configurator::getState() { return this->conf->getState(); }

Chip Configurator::getChip() {
  return this->chips->getAll()[atoi(this->chipIndex)];
}

Configurator::~Configurator() {
  if (this->allCustomParameters != NULL) {
    free(this->allCustomParameters);
  }
  if (this->dnsServer != NULL) {
    this->dnsServer->stop();
    free(this->dnsServer);
  }
  if (this->conf != NULL) {
    free(this->conf);
  }
}
