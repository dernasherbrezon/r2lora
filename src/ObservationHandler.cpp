#include "ObservationHandler.h"

#include <ArduinoJson.h>
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
    this->sendFailure("unable to parse request");
    return;
  }
  log_i("received observation request on: %fMhz", req.getFreq());
  int code = lora->begin(&req);
  if (code != 0) {
    this->sendFailure("unable to start lora");
    return;
  }
  this->receiving = true;
  this->sendSuccess();
}

void ObservationHandler::handlePull() {
  DynamicJsonDocument json(2048);
  json["status"] = "SUCCESS";
  JsonArray frames = json.createNestedArray("frames");
  for (size_t i = 0; i < this->receivedFrames.size(); i++) {
    JsonObject obj = frames.createNestedObject();
    LoRaFrame *curFrame = this->receivedFrames[i];
    obj["dataLength"] = curFrame->getDataLength();
    obj["data"] = curFrame->getData();
    obj["rssi"] = curFrame->getRssi();
    obj["snr"] = curFrame->getSnr();
  }
  String output;
  serializeJson(json, output);
  this->web->send(200, "application/json; charset=UTF-8", output);
}

void ObservationHandler::handleStop() {
  if (!this->receiving) {
    return;
  }
  this->receiving = false;
  log_i("stop observation");
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

void ObservationHandler::sendFailure(const char *message) {
  StaticJsonDocument<128> json;
  json["status"] = "FAILURE";
  json["failureMessage"] = message;
  String output;
  serializeJson(json, output);
  this->web->send(200, "application/json; charset=UTF-8", output);
}

void ObservationHandler::sendSuccess() {
  StaticJsonDocument<128> json;
  json["status"] = "SUCCESS";
  String output;
  serializeJson(json, output);
  this->web->send(200, "application/json; charset=UTF-8", output);
}