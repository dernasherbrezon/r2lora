#ifndef Display_h
#define Display_h

#include <LoRaModule.h>
#include <SSD1306Wire.h>
#include <Wire.h>

class Display {
 public:
  ~Display();
  void init();
  void setStatus(const char *status);
  void setStationName(const char *stationName);
  void setIpAddress(String ipAddress);
  void update();

 private:
  const char *status = NULL;
  const char *stationName = NULL;
  String ipAddress;
  SSD1306Wire *display = NULL;
};

#endif