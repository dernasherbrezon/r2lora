#ifndef Configurator_h
#define Configurator_h

#define STRING_LEN 256

#include <IotWebConf.h>
#include <IotWebConfUsing.h>

#include "Chips.h"

class Configurator {
 public:
  Configurator(WebServer *webServer);
  ~Configurator();

  void setOnConfiguredCallback(std::function<void()> func);
  Chip getChip();
  iotwebconf::NetworkState getState();

  void loop();

 private:
  DNSServer *dnsServer = NULL;
  IotWebConf *conf = NULL;
  Chips *chips = NULL;

  IotWebConfParameterGroup *allCustomParameters = NULL;
  IotWebConfSelectParameter *chipType = NULL;
  char chipIndex[STRING_LEN];
  bool configured = false;
};

#endif
