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
  const char *getUsername();
  const char *getPassword();
  const char *getNtpServer();

 private:
  DNSServer *dnsServer = NULL;
  IotWebConf *conf = NULL;
  Chips *chips = NULL;

  IotWebConfParameterGroup *allCustomParameters = NULL;
  IotWebConfSelectParameter *chipType = NULL;

  char chipIndex[STRING_LEN];
  char apiUsername[IOTWEBCONF_WORD_LEN];
  char apiPassword[IOTWEBCONF_PASSWORD_LEN];
  char ntpServer[STRING_LEN];
  bool configured = false;
  IotWebConfTextParameter apiUsernameParameter =
    IotWebConfTextParameter("API username", "apiUsername", this->apiUsername, IOTWEBCONF_WORD_LEN);
  IotWebConfPasswordParameter apiPasswordParameter =
    IotWebConfPasswordParameter("API password", "apiPassword", this->apiPassword, IOTWEBCONF_PASSWORD_LEN);
  IotWebConfTextParameter ntpServerParameter =
    IotWebConfTextParameter("NTP server", "ntpServer", this->ntpServer, STRING_LEN, "pool.ntp.org");

};

#endif
