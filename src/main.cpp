#include <Arduino.h>
#include <WiFiServer.h>
#include <esp32-hal-log.h>

#include "Configurator.h"
#include "LoRaModule.h"
#include "ObservationHandler.h"

Configurator *conf;
LoRaModule *lora;
WebServer *web;
ObservationHandler *observationHandler;

void setupRadio() {
  log_i("connected");

  // FIXME shutdown lora?
  // FIXME reset?
  lora->setup(conf->getBoard(), conf->getChip());

  // radio = new SX1278(new Module(1, 1, 1, 1));
  // if( SX1278 *sx = dynamic_cast<SX1278 *>(radio)) {
  // sx->begin();
  // }
}

void handleStatus() {
  // FIXME return json
  // status LoraModule status
}

void setup() {
  log_i("starting");
  lora = new LoRaModule();
  web = new WebServer(80);
  conf = new Configurator(web);
  conf->setOnConfiguredCallback([] { setupRadio(); });

  observationHandler = new ObservationHandler(web, lora);

  web->on("/status", HTTP_GET, []() { handleStatus(); });
}

void loop() {
  conf->loop();
  observationHandler->loop();
}
