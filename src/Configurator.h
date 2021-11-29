#ifndef Configurator_h
#define Configurator_h

#include <IotWebConf.h>
#include <IotWebConfUsing.h>
#include "Boards.h"
#include "Board.h"

class Configurator {
 public:
  Configurator();
  ~Configurator();

  void setOnConfiguredCallback(std::function<void()> func);
  Board getBoard();
  // FIXME delete?
  bool isConfigured();

  void loop();

 private:
  DNSServer *dnsServer;
  WebServer *webServer;
  IotWebConf *conf;
  Boards *boards;

  IotWebConfParameterGroup *allCustomParameters;
  IotWebConfSelectParameter *boardType;
  char boardIndex[STRING_LEN];
  bool configured;
};

#endif
