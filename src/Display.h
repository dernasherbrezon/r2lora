#ifndef Display_h
#define Display_h

#include <LoRaModule.h>
#include <SSD1306Wire.h>
#include <Wire.h>

class Display {
 public:
  ~Display();
  void init();
  void updateStatus(const char *status);
  void updateStationName(const char *stationName);
  void updateIpAddress(String ipAddress);

 private:
  SSD1306Wire *display = NULL;
};

#define logo_width 32
#define logo_height 32
static uint8_t logo_bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00,
    0xFC, 0x00, 0x00, 0x00, 0xFE, 0x01, 0x00, 0x00, 0xFC, 0x01, 0x00, 0x00,
    0xF8, 0x07, 0x00, 0x00, 0xF0, 0x07, 0x00, 0x00, 0xE0, 0x0F, 0x00, 0x00,
    0xC0, 0x3F, 0x00, 0x00, 0x80, 0x7F, 0x00, 0x00, 0x00, 0x3F, 0x01, 0x00,
    0x00, 0xBE, 0x07, 0x00, 0x00, 0xDC, 0x07, 0x00, 0x00, 0xE8, 0x0F, 0x00,
    0x00, 0xF0, 0x0F, 0x00, 0x00, 0xF8, 0x0F, 0x00, 0x00, 0xFE, 0x27, 0x00,
    0x00, 0xFE, 0x3B, 0x00, 0x00, 0xFC, 0xFD, 0x00, 0x00, 0x7C, 0xFE, 0x01,
    0x00, 0xF0, 0xFC, 0x01, 0x01, 0x41, 0xF8, 0x07, 0xB2, 0x01, 0xF0, 0x07,
    0x22, 0x03, 0xE0, 0x1F, 0x66, 0x00, 0xC0, 0x1F, 0xC4, 0x01, 0x80, 0x3F,
    0x1C, 0x02, 0x00, 0x7F, 0x18, 0x00, 0x00, 0x3E, 0x70, 0x00, 0x00, 0x1C,
    0xC0, 0x01, 0x00, 0x08, 0x00, 0x01, 0x00, 0x00,
};

#endif