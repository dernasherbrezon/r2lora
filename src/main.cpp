#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFiServer.h>
#include <esp32-hal-log.h>

#include "Configurator.h"
#include "LoRaModule.h"
#include "ObservationHandler.h"
#include "time.h"

Configurator *conf;
LoRaModule *lora;
WebServer *web;
ObservationHandler *observationHandler;

void setupRadio() {
  log_i("r2lora configured. initializing LoRa module and API");
  if (observationHandler != NULL) {
    log_i("reset API configuration");
    delete observationHandler;
  }
  if (lora != NULL) {
    log_i("reset LoRa configuration");
    delete lora;
  }
  lora = new LoRaModule();
  observationHandler = new ObservationHandler(web, lora, conf->getUsername(),
                                              conf->getPassword());
  configTime(0, 0, conf->getNtpServer());
  lora->setup(conf->getChip());
}

void handleStatus() {
  if (!web->authenticate(conf->getUsername(), conf->getPassword())) {
    web->requestAuthentication();
    return;
  }
  // FIXME add temperature?
  StaticJsonDocument<128> json;
  // INIT - waiting for AP to initialize
  // CONNECTING - connecting to WiFi
  // RECEIVING - lora is listening for data
  // IDLE - module is waiting for new observation
  if (lora == NULL) {
    json["status"] = "INIT";
  } else if (conf->getState() == iotwebconf::NetworkState::Connecting) {
    json["status"] = "CONNECTING";
  } else if (lora->isReceivingData()) {
    json["status"] = "RECEIVING";
  } else {
    json["status"] = "IDLE";
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
  observationHandler = new ObservationHandler(web, lora, conf->getUsername(),
                                              conf->getPassword());

  web->on("/status", HTTP_GET, []() { handleStatus(); });
}

void loop() {
  conf->loop();
  observationHandler->loop();
}
