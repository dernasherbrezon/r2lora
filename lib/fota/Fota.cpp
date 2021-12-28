#include "Fota.h"

#include <ArduinoJson.h>
#include <Update.h>
#include <esp32-hal-log.h>
#include <time.h>

// can't be less than 32768
// the output buffer must be a power of 2 size that is at least as large as the LZ dictionary (typically 32KB)
// https://github.com/richgel999/miniz/issues/1
#define UNCOMPRESSED_BUFFER_LENGTH 32768

int Fota::loop(bool reboot) {
  if (this->client == NULL) {
    return FOTA_NO_UPDATES;
  }
  unsigned long currentTime = millis();
  if (currentTime < this->nextUpdateTime) {
    return FOTA_NO_UPDATES;
  }
  log_i("checking new version");
  if (!this->client->begin(hostname, port, this->indexFile)) {
    log_e("unable to begin");
    return FOTA_UNKNOWN_ERROR;
  }
  if (!this->lastModified.isEmpty()) {
    this->client->addHeader("If-Modified-Since", this->lastModified);
  }
  const char *headers[] = {"Last-Modified"};
  this->client->collectHeaders(headers, 1);
  int code = this->client->GET();
  if (code < 0) {
    log_e("unable to connect to: %s", this->hostname);
    this->nextUpdateTime = currentTime + this->updateInterval;
    this->currentRetry = 0;
    return FOTA_UNKNOWN_ERROR;
  }
  if (code == 304 || code == 404) {
    log_i("no firmware updates");
    this->nextUpdateTime = currentTime + this->updateInterval;
    this->currentRetry = 0;
    return FOTA_NO_UPDATES;
  }
  if (code != 200) {
    if (this->currentRetry >= this->maxRetry) {
      log_i("invalid response code: %d. no more retry", code);
      // next update will be on the next day
      this->nextUpdateTime = currentTime + this->updateInterval;
      this->currentRetry = 0;
      return FOTA_INVALID_SERVER_RESPONSE;
    }
    this->currentRetry++;
    //linear backoff with random jitter
    this->nextUpdateTime += this->currentRetry * 1000 + random(500);
    log_i("invalid response code: %d. retry", code);
    return FOTA_RETRY;
  }

  this->nextUpdateTime = currentTime + this->updateInterval;
  this->currentRetry = 0;
  this->lastModified = this->client->header("Last-Modified");

  int size = this->client->getSize();
  if (size < 0) {
    log_i("invalid Content-Length header. Expected positive length");
    return FOTA_INVALID_SERVER_RESPONSE;
  }
  // arduinojson need some more memory than raw json
  DynamicJsonDocument json(2 * size);
  DeserializationError error = deserializeJson(json, this->client->getStream());
  this->client->end();
  if (error) {
    log_e("unable to read fota index: %s", error.c_str());
    return FOTA_INVALID_SERVER_RESPONSE;
  }
  const char *filename = NULL;
  const char *md5Checksum = NULL;
  size_t uncompressedSize = 0;
  JsonArray array = json.as<JsonArray>();
  for (size_t i = 0; i < array.size(); i++) {
    JsonObject cur = array[i];
    const char *curName = cur["name"];
    if (strncmp(curName, this->fotaName, MAX_FIELD_LENGTH) != 0) {
      continue;
    }
    const char *curVersion = cur["version"];
    if (strncmp(curVersion, this->currentVersion, MAX_FIELD_LENGTH) == 0) {
      log_i("no new version found");
      return FOTA_NO_UPDATES;
    }

    filename = cur["filename"];
    md5Checksum = cur["md5Checksum"];
    uncompressedSize = cur["size"];
    log_i("new version available: %s", curVersion);
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
  if (uncompressedSize == 0) {
    log_i("invalid uncompressed size received: %zu", uncompressedSize);
    return FOTA_INVALID_SERVER_RESPONSE;
  }

  code = downloadAndApplyFirmware(filename, md5Checksum, uncompressedSize);
  this->client->end();
  if (code != 0) {
    return code;
  }

  if (reboot) {
    log_i("update completed. rebooting");
    ESP.restart();
  } else {
    log_i("update completed");
  }
  return FOTA_SUCCESS;
}
void Fota::init(const char *currentVersion, const char *hostname, unsigned short port, const char *indexFile, unsigned long updateInterval, const char *fotaName) {
  if (this->client != NULL) {
    log_e("fota was already initialized");
    return;
  }
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
  this->compressedBuffer = (uint8_t *)malloc(sizeof(uint8_t) * SPI_FLASH_SEC_SIZE);
  this->uncompressedBuffer = (uint8_t *)malloc(sizeof(uint8_t) * UNCOMPRESSED_BUFFER_LENGTH);
  this->inflator = (tinfl_decompressor *)malloc(sizeof(tinfl_decompressor));

  log_i("fota initialized");
}
void Fota::deinit() {
  if (this->client == NULL) {
    return;
  }
  this->client->end();
  delete this->client;
  this->client = NULL;
  if (this->compressedBuffer != NULL) {
    free(this->compressedBuffer);
    this->compressedBuffer = NULL;
  }
  if (this->uncompressedBuffer != NULL) {
    free(this->uncompressedBuffer);
    this->uncompressedBuffer = NULL;
  }
  if (this->inflator != NULL) {
    free(this->inflator);
    this->inflator = NULL;
  }
  log_i("fota stopped");
}

void Fota::setOnUpdate(std::function<void(size_t, size_t)> func) {
  this->onUpdateFunc = func;
}

int Fota::downloadAndApplyFirmware(const char *filename, const char *md5Checksum, size_t uncompressedSize) {
  if (!this->client->begin(this->hostname, this->port, filename)) {
    log_e("unable to start downloading");
    return FOTA_UNKNOWN_ERROR;
  }

  int code = this->client->GET();
  if (code != 200) {
    log_i("unable to download firmware: %s code %d", filename, code);
    this->client->end();
    return FOTA_INVALID_SERVER_RESPONSE;
  }

  int size = this->client->getSize();
  if (size <= 0) {
    log_i("invalid content length: %d", size);
    this->client->end();
    return FOTA_INVALID_SERVER_RESPONSE;
  }

  if (!Update.begin(uncompressedSize)) {
    log_i("not enough space for firmware upgrade. required: %d", uncompressedSize);
    this->client->end();
    return FOTA_INTERNAL_ERROR;
  }

  if (this->onUpdateFunc) {
    Update.onProgress(this->onUpdateFunc);
  }
  if (!Update.setMD5(md5Checksum)) {
    log_i("invalid md5sum configured on the server: %s", md5Checksum);
    Update.abort();
    this->client->end();
    return FOTA_INVALID_SERVER_RESPONSE;
  }

  log_i("downloading new firmware: %d bytes", size);

  code = writeGzippedStream(this->client->getStream(), size);
  if (code != FOTA_SUCCESS) {
    Update.abort();
    this->client->end();
    return code;
  }
  this->client->end();

  if (!Update.end()) {
    log_e("unable to complete update: %s", Update.errorString());
    return FOTA_INTERNAL_ERROR;
  }

  return FOTA_SUCCESS;
}

int Fota::writeGzippedStream(Stream &data, int compressedSize) {
  tinfl_init(this->inflator);
  uint8_t *nextCompressedBuffer = this->compressedBuffer;
  uint8_t *nextUncompressedBuffer = this->uncompressedBuffer;
  ssize_t availableOut = UNCOMPRESSED_BUFFER_LENGTH;
  tinfl_status status = TINFL_STATUS_NEEDS_MORE_INPUT;
  int remainingCompressed = compressedSize;
  size_t actuallyRead = 0;
  int result = FOTA_SUCCESS;
  while (true) {
    int flags = TINFL_FLAG_PARSE_ZLIB_HEADER;
    // read next batch only when the previous input was processed
    if (actuallyRead == 0) {
      ssize_t bytesToRead;
      if (remainingCompressed > SPI_FLASH_SEC_SIZE) {
        bytesToRead = SPI_FLASH_SEC_SIZE;
      } else {
        bytesToRead = remainingCompressed;
      }
      actuallyRead = data.readBytes(this->compressedBuffer, bytesToRead);
      if (actuallyRead == 0) {
        log_e("unable to read %zu: %zu", bytesToRead, actuallyRead);
        result = FOTA_INVALID_SERVER_RESPONSE;
        break;
      }
      //reset pointer so that decompress starts from the beginning
      nextCompressedBuffer = this->compressedBuffer;
      remainingCompressed -= actuallyRead;
    }

    if (remainingCompressed > 0) {
      flags |= TINFL_FLAG_HAS_MORE_INPUT;
    }

    // inBytes and outBytes will contain of how many input bytes were actually processed
    // and how many output bytes were actually produced
    size_t inBytes = actuallyRead;
    size_t outBytes = availableOut;
    status = tinfl_decompress(inflator, (const mz_uint8 *)nextCompressedBuffer, &inBytes,
                              this->uncompressedBuffer, nextUncompressedBuffer, &outBytes,
                              flags);
    actuallyRead -= inBytes;
    nextCompressedBuffer = nextCompressedBuffer + inBytes;

    availableOut -= outBytes;
    nextUncompressedBuffer = nextUncompressedBuffer + outBytes;

    if ((status <= TINFL_STATUS_DONE) || (availableOut <= 0)) {
      size_t bytesInTheOutput = UNCOMPRESSED_BUFFER_LENGTH - availableOut;
      size_t actuallyWrote = Update.write(this->uncompressedBuffer, bytesInTheOutput);
      if (actuallyWrote != bytesInTheOutput) {
        log_e("unable to write %zu bytes: %s", bytesInTheOutput, Update.errorString());
        result = FOTA_UNKNOWN_ERROR;
        break;
      }
      //reset pointer to the output
      nextUncompressedBuffer = this->uncompressedBuffer;
      availableOut = UNCOMPRESSED_BUFFER_LENGTH;
    }

    if (status <= TINFL_STATUS_DONE) {
      if (status != TINFL_STATUS_DONE) {
        result = FOTA_INVALID_ZLIB_FILE;
        log_e("unable to decompress: %d", status);
      }
      break;
    }
  }
  return result;
}

Fota::~Fota() {
  this->deinit();
}
