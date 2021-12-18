#ifndef AutoUpdater_h
#define AutoUpdater_h

#include <HTTPClient.h>
#include <stddef.h>

#include <functional>

class AutoUpdater {
 public:

  void init(const char *hostname, unsigned short port, unsigned long updateInterval);
  void deinit();
  void loop();

  void setOnUpdate(std::function<void(size_t, size_t)> func);

 private:
  HTTPClient *client = NULL;
  String lastModified;
  std::function<void(size_t, size_t)> onUpdateFunc;
};

#endif