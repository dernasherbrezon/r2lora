#include "AutoUpdater.h"

#include <ArduinoJson.h>
#include <Update.h>
#include <esp32-hal-log.h>
#include <time.h>

static const char *FIRMWARE_INDEX = "/fota/r2lora.json";

void AutoUpdater::loop() {
  if (this->client == NULL) {
    return;
  }
  unsigned long currentTime = millis();
  if (currentTime - this->updateInterval < this->lastUpdateTime) {
    return;
  }
  log_i("time for auto update");
  if (!this->client->begin(hostname, port, FIRMWARE_INDEX)) {
    log_e("unable to begin");
    return;
  }
  if (!this->lastModified.isEmpty()) {
    this->client->addHeader("If-Modified-Since", this->lastModified);
  }
  int code = this->client->GET();
  if (code == 304 || code == 404) {
    log_i("no firmware updates");
    this->lastUpdateTime = currentTime;
    this->currentRetry = 0;
    return;
  }
  if (code != 200) {
    if (this->currentRetry >= this->maxRetry) {
      log_i("invalid response code: %d. no more retry", code);
      // next update will be on the next day
      this->lastUpdateTime += this->updateInterval;
      this->currentRetry = 0;
      return;
    }
    this->currentRetry++;
    //linear backoff with random jitter
    this->lastUpdateTime += this->currentRetry * 1000 + random(500);
    log_i("invalid response code: %d. retry", code);
    return;
  }

  this->lastUpdateTime = currentTime;
  this->currentRetry = 0;
  this->lastModified = this->client->header("Last-Modified");

  int size = this->client->getSize();
  if (size < 0) {
    log_i("invalid Content-Length header. Expected positive length");
    return;
  }
  DynamicJsonDocument json(size);
  DeserializationError error = deserializeJson(json, this->client->getStream());
  this->client->end();
  if (error) {
    log_e("invalid json received: %s", error.c_str());
    return;
  }
  const char *filename = NULL;
  const char *md5Checksum = NULL;
  JsonArray array = json.to<JsonArray>();
  for (size_t i = 0; i < array.size(); i++) {
    JsonObject cur = array[i];
    if (strncmp(cur["name"], this->fotaName, MAX_FIELD_LENGTH) != 0) {
      continue;
    }
    if (strncmp(cur["version"], FIRMWARE_VERSION, MAX_FIELD_LENGTH) == 0) {
      log_i("no new version for update");
      return;
    }

    filename = cur["filename"];
    md5Checksum = cur["md5Checksum"];
    break;
  }
  if (filename == NULL) {
    log_i("can't find firmware on server for: %s", this->fotaName);
    return;
  }

  code = downloadAndApplyFirmware(filename, md5Checksum);
  this->client->end();
  if (code != 0) {
    return;
  }

  log_i("update completed. rebooting");
  ESP.restart();
}
void AutoUpdater::init(const char *hostname, unsigned short port, unsigned long updateInterval, const char *fotaName) {
  this->fotaName = fotaName;
  this->updateInterval = updateInterval;
  this->client = new HTTPClient();
  this->client->setTimeout(10000);
  this->client->setConnectTimeout(10000);
  this->client->setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
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

int AutoUpdater::downloadAndApplyFirmware(const char *filename, const char *md5Checksum) {
  if (!this->client->begin(this->hostname, this->port, filename)) {
    log_e("unable to start downloading");
    return -1;
  }

  // gzip is not supported on ESP32
  // there is rom/miniz.h that can potentially used, but
  // it is stripped version of https://github.com/richgel999/miniz
  int code = this->client->GET();
  if (code != 200) {
    log_i("unable to download firmware: %s code %d", filename, code);
    this->client->end();
    return -1;
  }

  int size = this->client->getSize();
  if (size <= 0) {
    log_i("invalid content length: %d", size);
    return -1;
  }

  if (!Update.begin(size)) {
    log_i("not enough space for firmware upgrade. required: %d", size);
    return -1;
  }

  if (this->onUpdateFunc) {
    Update.onProgress(this->onUpdateFunc);
  }
  Update.setMD5(md5Checksum);

  size_t actuallyWritten = Update.writeStream(this->client->getStream());
  if (actuallyWritten != size) {
    log_e("number of bytes written doesn't match the expected: %s", Update.errorString());
    return -1;
  }

  if (!Update.end()) {
    log_e("unable to complete update: %s", Update.errorString());
    return -1;
  }

  return 0;
}
