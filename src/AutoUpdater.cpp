#include "AutoUpdater.h"

#include <esp32-hal-log.h>

void AutoUpdater::loop() {
  if (this->client == NULL) {
    return;
  }
  //FIXME check update interval
  if (!this->lastModified.isEmpty()) {
    this->client->addHeader("If-Modified-Since", this->lastModified);
  }
  int code = this->client->GET();
  if (code < 0) {
    log_e("unable to get updates: %d", code);
    return;
  }
  if (code == 304) {
    log_i("no firmware updates");
    return;
  }
  this->lastModified = this->client->header("Last-Modified");

  //FIXME
}
void AutoUpdater::init(const char *hostname, unsigned short port, unsigned long updateInterval) {
  this->client = new HTTPClient();
  this->client->setTimeout(10000);
  this->client->setConnectTimeout(10000);
  this->client->setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  this->client->begin(hostname, port, "/ota.txt??");
  log_i("auto update initialized");
}
void AutoUpdater::deinit() {
  if (this->client == NULL) {
    return;
  }
  this->client->end();
  delete this->client;
  this->client = NULL;
  log_i("auto update stopped");
}

void AutoUpdater::setOnUpdate(std::function<void(size_t, size_t)> func) {
  this->onUpdateFunc = func;
}
