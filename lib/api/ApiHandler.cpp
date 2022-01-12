#include "ApiHandler.h"

#include <ArduinoJson.h>
#include <Util.h>
#include <esp32-hal-log.h>

#include "JsonHandler.h"

ApiHandler::ApiHandler(WebServer *web, LoRaModule *lora, Configurator *config) {
  this->web = web;
  this->lora = lora;

  std::function<int(String &, String *)> startFunc = std::bind(&ApiHandler::handleStart, this, std::placeholders::_1, std::placeholders::_2);
  this->web->addHandler(new JsonHandler(startFunc, "/lora/rx/start", HTTP_POST, config));

  std::function<int(String &, String *)> stopFunc = std::bind(&ApiHandler::handleStop, this, std::placeholders::_1, std::placeholders::_2);
  this->web->addHandler(new JsonHandler(stopFunc, "/lora/rx/stop", HTTP_POST, config));

  std::function<int(String &, String *)> pullFunc = std::bind(&ApiHandler::handlePull, this, std::placeholders::_1, std::placeholders::_2);
  this->web->addHandler(new JsonHandler(pullFunc, "/lora/rx/pull", HTTP_GET, config));

  std::function<int(String &, String *)> txFunc = std::bind(&ApiHandler::handleTx, this, std::placeholders::_1, std::placeholders::_2);
  this->web->addHandler(new JsonHandler(txFunc, "/lora/tx/send", HTTP_POST, config));

  std::function<int(String &, String *)> startFskFunc = std::bind(&ApiHandler::handleFskStart, this, std::placeholders::_1, std::placeholders::_2);
  this->web->addHandler(new JsonHandler(startFunc, "/fsk/rx/start", HTTP_POST, config));

  std::function<int(String &, String *)> txFskFunc = std::bind(&ApiHandler::handleFskTx, this, std::placeholders::_1, std::placeholders::_2);
  this->web->addHandler(new JsonHandler(txFunc, "/fsk/tx/send", HTTP_POST, config));

  log_i("api handler was initialized");
}

int ApiHandler::handleFskStart(String &body, String *output) {
  if (body.isEmpty()) {
    this->sendFailure("request is empty", output);
    return 200;
  }
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, body);
  if (error) {
    this->sendFailure("unable to parse request", output);
    return 200;
  }
  FskState req;
  req.br = doc["br"];
  req.freq = doc["freq"];
  req.freqDev = doc["freqDev"];
  req.gain = doc["gain"];
  req.power = 10;
  req.preambleLength = doc["preambleLength"];
  req.rxBw = doc["rxBw"];
  req.sh = doc["sh"];
  if (doc.containsKey("syncWordLength")) {
    req.syncWordLength = doc["syncWordLength"];
    const char *data = doc["syncWord"];
    uint8_t *binaryData = NULL;
    int code = convertStringToHex(data, &binaryData, &req.syncWordLength);
    if (code != 0) {
      this->sendFailure("invalid hexadecimal string in the syncWord field", output);
      return 200;
    }
    req.syncWord = binaryData;
  } else {
    req.syncWord = NULL;
  }
  log_i("received fsk rx request on: %fMhz", req.freq);
  int code = lora->startFskRx(&req);
  if (req.syncWord != NULL) {
    free(req.syncWord);
  }
  if (code != 0) {
    this->sendFailure("unable to start fsk", output);
    return 200;
  }
  this->receiving = true;
  this->sendSuccess(output);
  return 200;
}
int ApiHandler::handleFskTx(String &body, String *output) {
  if (body.isEmpty()) {
    this->sendFailure("request is empty", output);
    return 200;
  }
  if (lora->isReceivingData()) {
    this->sendFailure("cannot transmit during receive", output);
    return 200;
  }
  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, body);
  if (error) {
    this->sendFailure("unable to parse tx request", output);
    return 200;
  }
  FskState req;
  const char *data = doc["data"];
  req.power = doc["power"];
  req.freq = doc["freq"];   // = (434.0F)
  req.br = doc["bw"];       // = (125.0F)
  req.freqDev = doc["sf"];  // = (uint8_t)9U
  req.rxBw = doc["cr"];     // = (uint8_t)7U
  if (doc.containsKey("syncWordLength")) {
    req.syncWordLength = doc["syncWordLength"];
    const char *data = doc["syncWord"];
    uint8_t *binaryData = NULL;
    int code = convertStringToHex(data, &binaryData, &req.syncWordLength);
    if (code != 0) {
      this->sendFailure("invalid hexadecimal string in the syncWord field", output);
      return 200;
    }
    req.syncWord = binaryData;
  } else {
    req.syncWord = NULL;
  }
  req.sh = doc["sh"];
  req.preambleLength = doc["preambleLength"];  // = (uint16_t)8U
  req.gain = 0;
  uint8_t *binaryData = NULL;
  size_t binaryDataLength = 0;
  int code = convertStringToHex(data, &binaryData, &binaryDataLength);
  if (code != 0) {
    if (req.syncWord != NULL) {
      free(req.syncWord);
    }
    this->sendFailure("invalid hexadecimal string in the data field", output);
    return 200;
  }
  log_i("tx data: %s", data);
  code = lora->fskTx(binaryData, binaryDataLength, &req);
  free(binaryData);
  if (req.syncWord != NULL) {
    free(req.syncWord);
  }
  if (code != 0) {
    this->sendFailure("unable to transmit", output);
    return 200;
  }
  this->sendSuccess(output);
  return 200;
}

int ApiHandler::handleStart(String &body, String *output) {
  if (body.isEmpty()) {
    this->sendFailure("request is empty", output);
    return 200;
  }
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, body);
  if (error) {
    this->sendFailure("unable to parse request", output);
    return 200;
  }
  LoraState req;
  req.freq = doc["freq"];                      // = (434.0F)
  req.bw = doc["bw"];                          // = (125.0F)
  req.sf = doc["sf"];                          // = (uint8_t)9U
  req.cr = doc["cr"];                          // = (uint8_t)7U
  req.syncWord = doc["syncWord"];              // = (uint8_t)18U
  req.preambleLength = doc["preambleLength"];  // = (uint16_t)8U
  req.gain = doc["gain"];                      // = (uint8_t)0U
  req.ldro = doc["ldro"];                      // 0 - auto, 1 - enable, 2 - disable
  req.power = 10;
  log_i("received lora rx request on: %fMhz", req.freq);
  int code = lora->startLoraRx(&req);
  if (code != 0) {
    this->sendFailure("unable to start lora", output);
    return 200;
  }
  this->receiving = true;
  this->sendSuccess(output);
  return 200;
}

int ApiHandler::handleTx(String &body, String *output) {
  if (body.isEmpty()) {
    this->sendFailure("request is empty", output);
    return 200;
  }
  if (lora->isReceivingData()) {
    this->sendFailure("cannot transmit during receive", output);
    return 200;
  }
  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, body);
  if (error) {
    this->sendFailure("unable to parse tx request", output);
    return 200;
  }
  LoraState req;
  const char *data = doc["data"];
  req.power = doc["power"];
  req.freq = doc["freq"];                      // = (434.0F)
  req.bw = doc["bw"];                          // = (125.0F)
  req.sf = doc["sf"];                          // = (uint8_t)9U
  req.cr = doc["cr"];                          // = (uint8_t)7U
  req.syncWord = doc["syncWord"];              // = (uint8_t)18U
  req.preambleLength = doc["preambleLength"];  // = (uint16_t)8U
  req.gain = 0;
  uint8_t *binaryData = NULL;
  size_t binaryDataLength = 0;
  int code = convertStringToHex(data, &binaryData, &binaryDataLength);
  if (code != 0) {
    this->sendFailure("invalid hexadecimal string in the data field", output);
    return 200;
  }
  log_i("tx data: %s", data);
  code = lora->loraTx(binaryData, binaryDataLength, &req);
  free(binaryData);
  if (code != 0) {
    this->sendFailure("unable to transmit", output);
    return 200;
  }
  this->sendSuccess(output);
  return 200;
}

int ApiHandler::handlePull(String &body, String *output) {
  DynamicJsonDocument json(2048);
  json["status"] = "SUCCESS";
  JsonArray frames = json.createNestedArray("frames");
  for (size_t i = 0; i < this->receivedFrames.size(); i++) {
    JsonObject obj = frames.createNestedObject();
    LoRaFrame *curFrame = this->receivedFrames[i];
    char *data = NULL;
    int code = convertHexToString(curFrame->getData(), curFrame->getDataLength(), &data);
    if (code != 0) {
      this->sendFailure("unable to serialize data", output);
      return 200;
    }
    obj["data"] = data;
    free(data);
    obj["rssi"] = curFrame->getRssi();
    obj["snr"] = curFrame->getSnr();
    obj["frequencyError"] = curFrame->getFrequencyError();
    obj["timestamp"] = curFrame->getTimestamp();
  }
  serializeJson(json, *output);
  for (size_t i = 0; i < this->receivedFrames.size(); i++) {
    delete this->receivedFrames[i];
  }
  this->receivedFrames.clear();
  return 200;
}

int ApiHandler::handleStop(String &body, String *output) {
  if (!this->receiving) {
    this->sendSuccess(output);
    return 200;
  }
  this->receiving = false;
  lora->stopRx();
  return this->handlePull(body, output);
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

void ApiHandler::sendFailure(const char *message, String *output) {
  StaticJsonDocument<512> json;
  json["status"] = "FAILURE";
  json["failureMessage"] = message;
  serializeJson(json, *output);
  log_i("failure: %s", message);
}

void ApiHandler::sendSuccess(String *output) {
  StaticJsonDocument<128> json;
  json["status"] = "SUCCESS";
  serializeJson(json, *output);
}