#include <ArduinoJson.h>
#include <WiFiServer.h>
#include <esp32-hal-log.h>

#include "ApiHandler.h"
#include "AutoUpdater.h"
#include "Configurator.h"
#include "Display.h"
#include "LoRaModule.h"
#include "time.h"

#ifndef BOARD_NAME
#define BOARD_NAME "native"
#endif

Configurator *conf;
LoRaModule *lora;
WebServer *web;
ApiHandler *apiHandler;
Display *display;
AutoUpdater *updater;

const char *getStatus() {
  // INIT - waiting for AP to initialize
  // CONNECTING - connecting to WiFi
  // RECEIVING - lora is listening for data
  // IDLE - module is waiting for rx/tx requests
  if (conf->getState() == iotwebconf::NetworkState::Connecting) {
    return "CONNECTING";
  } else if (conf->getState() == iotwebconf::NetworkState::ApMode) {
    return "INIT";
  } else if (lora->isReceivingData()) {
    return "RECEIVING";
  } else {
    return "IDLE";
  }
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

  display = new Display();
  display->init();
  display->setStatus(getStatus());
  display->update();

  lora = new LoRaModule();
  lora->setOnRxStartedCallback([] {
    display->setStatus(getStatus());
    display->update();
  });
  lora->setOnRxStoppedCallback([] {
    display->setStatus(getStatus());
    display->update();
  });

  updater = new AutoUpdater();
  updater->setOnUpdate([](size_t current, size_t total) {
    display->setStatus("UPDATING");
    display->setProgress((uint8_t)((float)current / total));
    display->update();
  });

  conf = new Configurator(web);
  conf->setOnConfiguredCallback([] {
    log_i("configuration completed");
    configTime(0, 0, conf->getNtpServer());
    log_i("NTP initialized: %s", conf->getNtpServer());
    lora->init(conf->getChip());
    display->setStationName(conf->getDeviceName());
    display->setStatus(getStatus());
    display->update();
    if (conf->isAutoUpdate()) {
      updater->init(FIRMWARE_VERSION, "apt.r2server.ru", 80, "/fota/r2lora.json", 24 * 60 * 60 * 1000, BOARD_NAME);  // update once a day
    } else {
      updater->deinit();
    }
  });
  conf->setOnWifiConnectedCallback([] {
    log_i("wifi connected");
    display->setIpAddress(WiFi.localIP().toString());
    display->setStatus(getStatus());
    display->update();
  });

  apiHandler = new ApiHandler(web, lora, conf->getUsername(), conf->getPassword());

  web->on("/status", HTTP_GET, []() {
    handleStatus();
  });
}

void loop() {
  conf->loop();
  apiHandler->loop();
  updater->loop();
}
