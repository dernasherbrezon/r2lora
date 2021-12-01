
#include "Configurator.h"

static const char *LOG_TAG = "r2lora";

Configurator::Configurator(WebServer *webServer) {
  this->boards = new Boards();
  this->dnsServer = new DNSServer();
  this->conf = new IotWebConf(LOG_TAG, dnsServer, webServer, "", "0.1");

  this->boardType = new IotWebConfSelectParameter(
      "Board type", "boardType", this->boardIndex, STRING_LEN,
      this->boards->getBoardIndices(), this->boards->getBoardNames(),
      this->boards->getAll().size(), STRING_LEN);

  this->allCustomParameters =
      new IotWebConfParameterGroup("allCustomParameters", "r2lora");
  allCustomParameters->addItem(this->boardType);
  this->conf->addParameterGroup(this->allCustomParameters);
  //FIXME wifi connected callback

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

bool Configurator::isConfigured() { return this->configured; }

Board Configurator::getBoard() {
  return this->boards->getAll()[atoi(this->boardIndex)];
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
  if (this->boards != NULL) {
    free(this->boards);
  }
  if (this->conf != NULL) {
    free(this->conf);
  }
}
