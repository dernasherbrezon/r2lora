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
  // FIXME delete?
  bool isConfigured();

  void loop();

 private:
  DNSServer *dnsServer;
  IotWebConf *conf;
  Chips *chips;

  IotWebConfParameterGroup *allCustomParameters;
  IotWebConfSelectParameter *chipType;
  char chipIndex[STRING_LEN];

  bool configured;
};

#endif
