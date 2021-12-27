#ifndef Fota_h
#define Fota_h

#include <HTTPClient.h>
#include <stddef.h>

#include <functional>

#define MAX_FIELD_LENGTH 128

#define FOTA_SUCCESS 0
#define FOTA_UNKNOWN_ERROR -1
#define FOTA_NO_UPDATES -2
#define FOTA_INVALID_SERVER_RESPONSE -3
#define FOTA_RETRY -4
#define FOTA_INTERNAL_ERROR -5

class Fota {
 public:
  ~Fota();
  void init(const char *currentVersion, const char *hostname, unsigned short port, const char *indexFile, unsigned long updateInterval, const char *fotaName);
  void deinit();
  int loop(bool reboot);

  void setOnUpdate(std::function<void(size_t, size_t)> func);

 private:
  HTTPClient *client = NULL;
  const char *hostname = NULL;
  unsigned short port = 8080;
  const char *indexFile = NULL;
  const char *currentVersion = NULL;
  String lastModified;
  std::function<void(size_t, size_t)> onUpdateFunc;
  unsigned long nextUpdateTime = 0;
  unsigned long updateInterval = 0;
  uint8_t maxRetry = 3;
  uint8_t currentRetry = 0;
  const char *fotaName = NULL;
  uint8_t *compressedBuffer = NULL;
  uint8_t *uncompressedBuffer = NULL;

  int downloadAndApplyFirmware(const char *filename, const char *md5Checksum, size_t uncompressedSize);
  int writeGzippedStream(Stream &data, int compressedSize);
};

#endif