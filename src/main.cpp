#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFiServer.h>
#include <esp32-hal-log.h>

#include "ApiHandler.h"
#include "Configurator.h"
#include "Display.h"
#include "LoRaModule.h"
#include "time.h"

Configurator *conf;
LoRaModule *lora;
WebServer *web;
ApiHandler *apiHandler;
Display *display;

void setupRadio() {
  log_i("r2lora configured. initializing LoRa module and API");
  if (apiHandler != NULL) {
    log_i("reset API configuration");
    delete apiHandler;
  }
  if (lora != NULL) {
    log_i("reset LoRa configuration");
    delete lora;
  }
  // FIXME do not re-create. reset instead
  lora = new LoRaModule();
  apiHandler =
      new ApiHandler(web, lora, conf->getUsername(), conf->getPassword());
  configTime(0, 0, conf->getNtpServer());
  log_i("NTP initialized: %s", conf->getNtpServer());
  lora->setup(conf->getChip());
}

void handleStatus() {
  if (!web->authenticate(conf->getUsername(), conf->getPassword())) {
    web->requestAuthentication();
    return;
  }
  StaticJsonDocument<128> json;
  // INIT - waiting for AP to initialize
  // CONNECTING - connecting to WiFi
  // RECEIVING - lora is listening for data
  // IDLE - module is waiting for rx/tx requests
  if (lora == NULL) {
    json["status"] = "INIT";
  } else if (conf->getState() == iotwebconf::NetworkState::Connecting) {
    json["status"] = "CONNECTING";
  } else if (lora->isReceivingData()) {
    json["status"] = "RECEIVING";
  } else {
    json["status"] = "IDLE";
    int8_t sxTemperature;
    if (lora->getTempRaw(&sxTemperature) == 0) {
      json["loraTemperature"] = sxTemperature;
    }
  }

  String output;
  serializeJson(json, output);
  web->send(200, "application/json; charset=UTF-8", output);
}

void setup() {
  log_i("starting");
  web = new WebServer(80);
  conf = new Configurator(web);
  conf->setOnConfiguredCallback([] { setupRadio(); });

  lora = new LoRaModule();
  display = new Display(lora);
  display->init();
  apiHandler =
      new ApiHandler(web, lora, conf->getUsername(), conf->getPassword());

  web->on("/status", HTTP_GET, []() { handleStatus(); });
}

void loop() {
  conf->loop();
  apiHandler->loop();
  // display will wait some time
  display->loop();
}
