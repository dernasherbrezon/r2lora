#include "Fota.h"

#include <ArduinoJson.h>
#include <Update.h>
#include <esp32-hal-log.h>
#include <time.h>

int Fota::loop() {
  if (this->client == NULL) {
    return FOTA_SUCCESS;
  }
  unsigned long currentTime = millis();
  if (currentTime - this->updateInterval < this->lastUpdateTime) {
    return FOTA_SUCCESS;
  }
  log_i("time for auto update");
  if (!this->client->begin(hostname, port, this->indexFile)) {
    log_e("unable to begin");
    return FOTA_UNKNOWN_ERROR;
  }
  if (!this->lastModified.isEmpty()) {
    this->client->addHeader("If-Modified-Since", this->lastModified);
  }
  int code = this->client->GET();
  if (code < 0) {
    log_e("unable to connect to: %s", this->hostname);
    this->lastUpdateTime = currentTime;
    this->currentRetry = 0;
    return FOTA_UNKNOWN_ERROR;
  }
  if (code == 304 || code == 404) {
    log_i("no firmware updates");
    this->lastUpdateTime = currentTime;
    this->currentRetry = 0;
    return FOTA_NO_UPDATES;
  }
  if (code != 200) {
    if (this->currentRetry >= this->maxRetry) {
      log_i("invalid response code: %d. no more retry", code);
      // next update will be on the next day
      this->lastUpdateTime += this->updateInterval;
      this->currentRetry = 0;
      return FOTA_INVALID_SERVER_RESPONSE;
    }
    this->currentRetry++;
    //linear backoff with random jitter
    this->lastUpdateTime += this->currentRetry * 1000 + random(500);
    log_i("invalid response code: %d. retry", code);
    return FOTA_RETRY;
  }

  this->lastUpdateTime = currentTime;
  this->currentRetry = 0;
  this->lastModified = this->client->header("Last-Modified");

  int size = this->client->getSize();
  if (size < 0) {
    log_i("invalid Content-Length header. Expected positive length");
    return FOTA_INVALID_SERVER_RESPONSE;
  }
  DynamicJsonDocument json(size);
  DeserializationError error = deserializeJson(json, this->client->getStream());
  this->client->end();
  if (error) {
    log_e("invalid json received: %s", error.c_str());
    return FOTA_INVALID_SERVER_RESPONSE;
  }
  const char *filename = NULL;
  const char *md5Checksum = NULL;
  JsonArray array = json.as<JsonArray>();
  for (size_t i = 0; i < array.size(); i++) {
    JsonObject cur = array[i];
    const char *curName = cur["name"];
    if (strncmp(curName, this->fotaName, MAX_FIELD_LENGTH) != 0) {
      continue;
    }
    const char *curVersion = cur["version"];
    if (strncmp(curVersion, this->currentVersion, MAX_FIELD_LENGTH) == 0) {
      log_i("no new version for update");
      return FOTA_NO_UPDATES;
    }

    filename = cur["filename"];
    md5Checksum = cur["md5Checksum"];
    break;
  }
  if (filename == NULL) {
    log_i("can't find firmware on server for: %s", this->fotaName);
    return FOTA_NO_UPDATES;
  }
  if (md5Checksum == NULL) {
    log_i("md5checksum is missing for: %s", this->fotaName);
    return FOTA_INVALID_SERVER_RESPONSE;
  }

  code = downloadAndApplyFirmware(filename, md5Checksum);
  this->client->end();
  if (code != 0) {
    return code;
  }

  log_i("update completed. rebooting");
  ESP.restart();
  return FOTA_SUCCESS;
}
void Fota::init(const char *currentVersion, const char *hostname, unsigned short port, const char *indexFile, unsigned long updateInterval, const char *fotaName) {
  this->currentVersion = currentVersion;
  this->fotaName = fotaName;
  this->updateInterval = updateInterval;
  this->indexFile = indexFile;
  this->hostname = hostname;
  this->port = port;
  this->client = new HTTPClient();
  this->client->setTimeout(10000);
  this->client->setConnectTimeout(10000);
  this->client->setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  log_i("auto update initialized");
}
void Fota::deinit() {
  if (this->client == NULL) {
    return;
  }
  this->client->end();
  delete this->client;
  this->client = NULL;
  log_i("auto update stopped");
}

void Fota::setOnUpdate(std::function<void(size_t, size_t)> func) {
  this->onUpdateFunc = func;
}

int Fota::downloadAndApplyFirmware(const char *filename, const char *md5Checksum) {
  if (!this->client->begin(this->hostname, this->port, filename)) {
    log_e("unable to start downloading");
    return FOTA_UNKNOWN_ERROR;
  }

  // gzip is not supported on ESP32
  // there is rom/miniz.h that can potentially used, but
  // it is stripped version of https://github.com/richgel999/miniz
  int code = this->client->GET();
  if (code != 200) {
    log_i("unable to download firmware: %s code %d", filename, code);
    this->client->end();
    return FOTA_INVALID_SERVER_RESPONSE;
  }

  int size = this->client->getSize();
  if (size <= 0) {
    log_i("invalid content length: %d", size);
    return FOTA_INVALID_SERVER_RESPONSE;
  }

  if (!Update.begin(size)) {
    log_i("not enough space for firmware upgrade. required: %d", size);
    return FOTA_INTERNAL_ERROR;
  }

  if (this->onUpdateFunc) {
    Update.onProgress(this->onUpdateFunc);
  }
  if (!Update.setMD5(md5Checksum)) {
    return FOTA_INVALID_SERVER_RESPONSE;
  }

  size_t actuallyWritten = Update.writeStream(this->client->getStream());
  if (actuallyWritten != size) {
    log_e("number of bytes written doesn't match the expected: %s", Update.errorString());
    return FOTA_UNKNOWN_ERROR;
  }

  if (!Update.end()) {
    log_e("unable to complete update: %s", Update.errorString());
    return FOTA_INTERNAL_ERROR;
  }

  return FOTA_SUCCESS;
}
