
#include "Configurator.h"

static const char *LOG_TAG = "r2lora";

Configurator::Configurator(WebServer *webServer) {
  this->chips = new Chips();
  this->dnsServer = new DNSServer();
  this->conf = new IotWebConf(LOG_TAG, dnsServer, webServer, "", "0.1");
  this->conf->getThingNameParameter()->label =
      "Device name (Access point name, API username)";
  this->conf->getApPasswordParameter()->label = "Device password";

  this->chipType = new IotWebConfSelectParameter(
      "Chip type", "chipType", this->chipIndex, STRING_LEN,
      this->chips->getChipIndices(), this->chips->getChipNames(),
      this->chips->getAll().size(), STRING_LEN);

  this->allCustomParameters =
      new IotWebConfParameterGroup("allCustomParameters", LOG_TAG);
  allCustomParameters->addItem(this->chipType);
  allCustomParameters->addItem(&this->ntpServerParameter);
  this->conf->addParameterGroup(this->allCustomParameters);

  // c++ magic. bind class-based method to function
  std::function<boolean(iotwebconf::WebRequestWrapper *)> func =
      std::bind(&Configurator::formValidator, this, std::placeholders::_1);
  this->conf->setFormValidator(func);

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

iotwebconf::NetworkState Configurator::getState() {
  return this->conf->getState();
}

const char *Configurator::getUsername() { return this->conf->getThingName(); }
const char *Configurator::getPassword() {
  return this->conf->getApPasswordParameter()->valueBuffer;
}
const char *Configurator::getNtpServer() { return this->ntpServer; }

Chip Configurator::getChip() {
  return this->chips->getAll()[atoi(this->chipIndex)];
}

bool Configurator::formValidator(iotwebconf::WebRequestWrapper *web) {
  int length = web->arg(this->ntpServerParameter.getId()).length();
  bool valid = true;
  if (3 > length) {
    this->ntpServerParameter.errorMessage = "Should be at least 3 characters.";
    valid = false;
  }
  return valid;
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
