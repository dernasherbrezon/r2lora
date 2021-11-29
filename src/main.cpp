#include <Arduino.h>
#include <WiFiServer.h>

#include "Configurator.h"
#include "LoRaModule.h"

Configurator *conf;
LoRaModule *lora;

void setupRadio() {
  if (lora != NULL) {
    //FIXME shutdown first?
    //FIXME reset?
    free(lora);
  }
  lora = new LoRaModule();
  lora->setup(conf->getBoard());

  // radio = new SX1278(new Module(1, 1, 1, 1));
  // if( SX1278 *sx = dynamic_cast<SX1278 *>(radio)) {
  // sx->begin();
  // }
}

void setup() {
  conf = new Configurator();
  conf->setOnConfiguredCallback([] { setupRadio(); });
}

void loop() { conf->loop(); }
