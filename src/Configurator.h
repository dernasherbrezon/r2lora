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
  void setOnWifiConnectedCallback(std::function<void()> func);
  Chip getChip();
  iotwebconf::NetworkState getState();

  void loop();
  const char *getUsername();
  const char *getPassword();
  const char *getNtpServer();
  const char *getDeviceName();
  bool isAutoUpdate();

 private:
  DNSServer *dnsServer = NULL;
  IotWebConf *conf = NULL;
  Chips *chips = NULL;

  IotWebConfParameterGroup *allCustomParameters = NULL;
  IotWebConfSelectParameter *chipType = NULL;
  IotWebConfCheckboxParameter *autoUpdateParam = NULL;

  char chipIndex[STRING_LEN];
  char ntpServer[STRING_LEN] = "";
  char autoUpdate[STRING_LEN];
  bool configured = false;
  IotWebConfTextParameter ntpServerParameter = IotWebConfTextParameter("NTP server", "ntpServer", this->ntpServer, STRING_LEN, "pool.ntp.org");

  bool formValidator(iotwebconf::WebRequestWrapper *web);
};

#endif
