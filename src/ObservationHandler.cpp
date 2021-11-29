#include "ObservationHandler.h"

#include "JsonRequestHandler.h"
#include <esp32-hal-log.h>

ObservationHandler::ObservationHandler(WebServer *web, LoRaModule *lora) {
  this->web = web;
  this->web->on("/observation/start", HTTP_POST,
                [this]() { this->handleStart(); });
}

void ObservationHandler::handleStart() {
  String body = this->web->arg("plain");
  if (body == NULL) {
    return;
  }
  log_i("[r2lora] received: %s", body);
}