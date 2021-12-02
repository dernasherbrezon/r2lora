#include "ObservationHandler.h"

#include <esp32-hal-log.h>

ObservationHandler::ObservationHandler(WebServer *web, LoRaModule *lora) {
  this->web = web;
  this->lora = lora;
  this->web->on("/observation/start", HTTP_POST,
                [this]() { this->handleStart(); });
  this->web->on("/observation/stop", HTTP_POST,
                [this]() { this->handleStop(); });
  this->web->on("/observation/pull", HTTP_GET,
                [this]() { this->handlePull(); });
}

void ObservationHandler::handleStart() {
  String body = this->web->arg("plain");
  if (body == NULL) {
    return;
  }
  ObservationRequest req;
  int status = req.parseJson(body);
  if (status != 0) {
    // FIXME send invalid input error
    return;
  }
  log_i("received request: %f", req.getFreq());
  int code = lora->begin(&req);
  if (code != 0) {
    // FIXME respond with error
    return;
  }
  this->receiving = true;
}

void ObservationHandler::handlePull() {
  // FIXME serialize each LoraFrame and free them, clear receivedFrames
}

void ObservationHandler::handleStop() {
  if (!this->receiving) {
    return;
  }
  this->receiving = false;
  lora->end();
  this->handlePull();
}

void ObservationHandler::loop() {
  if (!this->receiving) {
    return;
  }
  LoRaFrame *frame = lora->loop();
  if (frame == NULL) {
    return;
  }
  this->receivedFrames.push_back(frame);
}