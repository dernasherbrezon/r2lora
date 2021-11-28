#include <Arduino.h>
#include <RadioLib.h>
#include "Configurator.h"

Configurator *conf;

void setup() {
  conf = new Configurator();
}

void loop() {
  conf->loop();
}
