#ifndef AutoUpdater_h
#define AutoUpdater_h

#include <HTTPClient.h>
#include <stddef.h>

#include <functional>

#include "Version.h"

#define MAX_FIELD_LENGTH 128

class AutoUpdater {
 public:
  void init(const char *hostname, unsigned short port, unsigned long updateInterval, const char *fotaName);
  void deinit();
  void loop();

  void setOnUpdate(std::function<void(size_t, size_t)> func);

 private:
  HTTPClient *client = NULL;
  const char *hostname = NULL;
  unsigned short port = 8080;
  String lastModified;
  std::function<void(size_t, size_t)> onUpdateFunc;
  unsigned long lastUpdateTime = 0;
  unsigned long updateInterval = 0;
  uint8_t maxRetry = 3;
  uint8_t currentRetry = 0;
  const char *fotaName = NULL;

  int downloadAndApplyFirmware(const char *filename);
};

#endif