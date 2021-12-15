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

const char *getStatus() {
  // INIT - waiting for AP to initialize
  // CONNECTING - connecting to WiFi
  // RECEIVING - lora is listening for data
  // IDLE - module is waiting for rx/tx requests
  if (lora == NULL) {
    return "INIT";
  } else if (conf->getState() == iotwebconf::NetworkState::Connecting) {
    return "CONNECTING";
  } else if (lora->isReceivingData()) {
    return "RECEIVING";
  } else {
    return "IDLE";
  }
}

void setupRadio() {
  log_i("configuration completed");
  configTime(0, 0, conf->getNtpServer());
  log_i("NTP initialized: %s", conf->getNtpServer());
  lora->init(conf->getChip());
}

void handleStatus() {
  if (!web->authenticate(conf->getUsername(), conf->getPassword())) {
    web->requestAuthentication();
    return;
  }
  StaticJsonDocument<128> json;
  json["status"] = getStatus();
  if (lora != NULL) {
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

  display = new Display();
  display->init();
  display->setStatus(getStatus());
  display->setIpAddress(WiFi.localIP().toString());
  display->update();

  conf->setOnConfiguredCallback([] {
    setupRadio();
    display->setStationName(conf->getDeviceName());
    display->setStatus(getStatus());
    display->update();
  });
  conf->setOnWifiConnectedCallback([] {
    display->setIpAddress(WiFi.localIP().toString());
    display->setStatus(getStatus());
    display->update();
  });

  lora = new LoRaModule();

  apiHandler = new ApiHandler(web, lora, conf->getUsername(), conf->getPassword());

  web->on("/status", HTTP_GET, []() {
    handleStatus();
  });
}

void loop() {
  conf->loop();
  apiHandler->loop();
}
