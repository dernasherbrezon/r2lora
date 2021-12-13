#include "ApiHandler.h"

#include <ArduinoJson.h>
#include <Util.h>
#include <esp32-hal-log.h>

ApiHandler::ApiHandler(WebServer *web, LoRaModule *lora,
                       const char *apiUsername, const char *apiPassword) {
  this->web = web;
  this->lora = lora;
  this->apiUsername = apiUsername;
  this->apiPassword = apiPassword;
  this->web->on("/rx/start", HTTP_POST, [this]() { this->handleStart(); });
  this->web->on("/rx/stop", HTTP_POST, [this]() { this->handleStop(); });
  this->web->on("/rx/pull", HTTP_GET, [this]() { this->handlePull(); });
  this->web->on("/tx/send", HTTP_POST, [this]() { this->handleTx(); });
}

void ApiHandler::handleStart() {
  if (!web->authenticate(apiUsername, apiPassword)) {
    web->requestAuthentication();
    return;
  }
  String body = this->web->arg("plain");
  if (body == NULL) {
    this->sendFailure("unable to parse request");
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

void ApiHandler::handleTx() {
  if (!web->authenticate(apiUsername, apiPassword)) {
    web->requestAuthentication();
    return;
  }
  if (lora->isReceivingData()) {
    this->sendFailure("cannot transmit during receive");
    return;
  }
  String body = this->web->arg("plain");
  if (body == NULL) {
    this->sendFailure("unable to parse tx request");
    return;
  }
  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, body);
  if (error) {
    log_e("unable to read json: %s", error.c_str());
    this->sendFailure("unable to parse tx request");
    return;
  }
  const char *data = doc["data"];
  int8_t power = doc["power"];
  uint8_t *output = NULL;
  size_t output_len = 0;
  int code = convertStringToHex(data, &output, &output_len);
  if (code != 0) {
    this->sendFailure("invalid hexadecimal string in the data field");
    return;
  }
  code = lora->tx(output, output_len, power);
  free(output);
  if (code != 0) {
    this->sendFailure("unable to transmit");
    return;
  }
  this->sendSuccess();
}

void ApiHandler::handlePull() {
  if (!web->authenticate(apiUsername, apiPassword)) {
    web->requestAuthentication();
    return;
  }
  DynamicJsonDocument json(2048);
  json["status"] = "SUCCESS";
  JsonArray frames = json.createNestedArray("frames");
  for (size_t i = 0; i < this->receivedFrames.size(); i++) {
    JsonObject obj = frames.createNestedObject();
    LoRaFrame *curFrame = this->receivedFrames[i];
    char *data = NULL;
    int code = convertHexToString(curFrame->getData(),
                                  curFrame->getDataLength(), &data);
    if (code != 0) {
      this->sendFailure("unable to serialize data");
      return;
    }
    obj["data"] = data;
    // FIXME verify that data was copied in the call above
    free(data);
    obj["rssi"] = curFrame->getRssi();
    obj["snr"] = curFrame->getSnr();
    obj["frequencyError"] = curFrame->getFrequencyError();
    obj["timestamp"] = curFrame->getTimestamp();
  }
  String output;
  serializeJson(json, output);
  this->web->send(200, "application/json; charset=UTF-8", output);
}

void ApiHandler::handleStop() {
  if (!web->authenticate(apiUsername, apiPassword)) {
    web->requestAuthentication();
    return;
  }
  if (!this->receiving) {
    this->sendSuccess();
    return;
  }
  this->receiving = false;
  log_i("stop observation");
  lora->end();
  this->handlePull();
}

void ApiHandler::loop() {
  if (!this->receiving) {
    return;
  }
  LoRaFrame *frame = lora->loop();
  if (frame == NULL) {
    return;
  }
  this->receivedFrames.push_back(frame);
}

void ApiHandler::sendFailure(const char *message) {
  StaticJsonDocument<512> json;
  json["status"] = "FAILURE";
  json["failureMessage"] = message;
  String output;
  serializeJson(json, output);
  this->web->send(200, "application/json; charset=UTF-8", output);
}

void ApiHandler::sendSuccess() {
  StaticJsonDocument<128> json;
  json["status"] = "SUCCESS";
  // FIXME serialize directly to WiFiClient
  String output;
  serializeJson(json, output);
  this->web->send(200, "application/json; charset=UTF-8", output);
}