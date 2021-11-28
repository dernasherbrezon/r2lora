#ifndef Configurator_h
#define Configurator_h

#include <IotWebConf.h>
#include <IotWebConfUsing.h>

#define STRING_LEN 128

class Configurator {
 public:
  Configurator();
  ~Configurator();

  void loop();

 private:
  DNSServer *dnsServer;
  WebServer *webServer;
  IotWebConf *conf;

  IotWebConfParameterGroup *allCustomParameters;
  IotWebConfSelectParameter *boardType;
  char chipType[STRING_LEN];
};

#endif
