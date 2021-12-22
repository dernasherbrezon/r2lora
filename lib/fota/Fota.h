#ifndef Fota_h
#define Fota_h

#include <HTTPClient.h>
#include <stddef.h>

#include <functional>

#define MAX_FIELD_LENGTH 128

class Fota {
 public:
  void init(const char *currentVersion, const char *hostname, unsigned short port, const char *indexFile, unsigned long updateInterval, const char *fotaName);
  void deinit();
  void loop();

  void setOnUpdate(std::function<void(size_t, size_t)> func);

 private:
  HTTPClient *client = NULL;
  const char *hostname = NULL;
  unsigned short port = 8080;
  const char *indexFile = NULL;
  const char *currentVersion = NULL;
  String lastModified;
  std::function<void(size_t, size_t)> onUpdateFunc;
  unsigned long lastUpdateTime = 0;
  unsigned long updateInterval = 0;
  uint8_t maxRetry = 3;
  uint8_t currentRetry = 0;
  const char *fotaName = NULL;

  int downloadAndApplyFirmware(const char *filename, const char *md5Checksum);
};

#endif