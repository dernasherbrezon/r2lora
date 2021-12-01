#ifndef Configurator_h
#define Configurator_h

#include <IotWebConf.h>
#include <IotWebConfUsing.h>
#include "Boards.h"
#include "Board.h"
#include "Chips.h"

class Configurator {
 public:
  Configurator(WebServer *webServer);
  ~Configurator();

  void setOnConfiguredCallback(std::function<void()> func);
  Board getBoard();
  Chip getChip();
  // FIXME delete?
  bool isConfigured();

  void loop();

 private:
  DNSServer *dnsServer;
  IotWebConf *conf;
  Boards *boards;
  Chips *chips;

  IotWebConfParameterGroup *allCustomParameters;
  IotWebConfSelectParameter *boardType;
  char boardIndex[STRING_LEN];
  IotWebConfSelectParameter *chipType;
  char chipIndex[STRING_LEN];

  bool configured;
};

#endif
