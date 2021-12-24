
#include "Configurator.h"

#include "Version.h"

Configurator::Configurator(WebServer *webServer) {
  this->chips = new Chips();
  this->dnsServer = new DNSServer();
  this->conf = new IotWebConf("r2lora", dnsServer, webServer, "", FIRMWARE_VERSION);
  this->conf->getThingNameParameter()->label = "Device name";
  this->conf->getApPasswordParameter()->label = "Device password";

  this->chipType = new IotWebConfSelectParameter(
      "Chip type", "chipType", this->chipIndex, STRING_LEN,
      this->chips->getChipIndices(), this->chips->getChipNames(),
      this->chips->getAll().size(), STRING_LEN);

  this->allCustomParameters = new IotWebConfParameterGroup("allCustomParameters", "r2lora");
  allCustomParameters->addItem(this->chipType);
  allCustomParameters->addItem(&this->autoUpdateParam);
  allCustomParameters->addItem(&this->ntpServerParameter);
  this->conf->addParameterGroup(this->allCustomParameters);

  // c++ magic. bind class-based method to function
  std::function<boolean(iotwebconf::WebRequestWrapper *)> func = std::bind(&Configurator::formValidator, this, std::placeholders::_1);
  this->conf->setFormValidator(func);

  this->configured = this->conf->init();
  log_i("configurator was initialized. config status: %d", this->configured);
  webServer->on("/", [this] {
    this->conf->handleConfig();
  });
  webServer->onNotFound([this]() {
    this->conf->handleNotFound();
  });
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

void Configurator::setOnWifiConnectedCallback(std::function<void()> func) {
  this->conf->setWifiConnectionCallback(func);
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
const char *Configurator::getUsername() {
  return IOTWEBCONF_ADMIN_USER_NAME;
}
const char *Configurator::getPassword() {
  return this->conf->getApPasswordParameter()->valueBuffer;
}
const char *Configurator::getNtpServer() {
  return this->ntpServer;
}
const char *Configurator::getDeviceName() {
  return this->conf->getThingNameParameter()->valueBuffer;
}
bool Configurator::isAutoUpdate() {
  return this->autoUpdateParam.isChecked();
}

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
