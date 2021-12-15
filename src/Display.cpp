#include "Display.h"

#include <WiFi.h>

// defined in platformio.ini
#ifndef PIN_OLED_SDA
#define PIN_OLED_SDA 0
#endif
#ifndef PIN_OLED_SCL
#define PIN_OLED_SCL 0
#endif

void Display::init() {
  if (PIN_OLED_SDA == 0 || PIN_OLED_SCL == 0) {
    return;
  }
  this->display = new SSD1306Wire(0x3c, PIN_OLED_SDA, PIN_OLED_SCL);
  this->display->init();
  this->display->drawXbm(0, 0, logo_width, logo_height, logo_bits);
  this->display->setFont(ArialMT_Plain_10);
  this->display->setTextAlignment(TEXT_ALIGN_LEFT);
  updateStationName(NULL);
  updateStatus(NULL);
  updateIpAddress("");
  // Used for OTA updates
  // this->display->drawProgressBar(0, logo_height + 20, 127, 5, 45);
  this->display->display();
}

void Display::updateStatus(const char *status) {
  if (status != NULL) {
    this->display->drawString(logo_width + 5, 21, "status: " + String(status));
  } else {
    this->display->drawString(logo_width + 5, 21, "status:               ");
  }
  this->display->display();
}
void Display::updateStationName(const char *stationName) {
  if (stationName != NULL) {
    this->display->drawString(logo_width + 5, 5, stationName);
  } else {
    this->display->drawString(logo_width + 5, 5, "r2lora              ");
  }
  this->display->display();
}
void Display::updateIpAddress(String ipAddress) {
  if (ipAddress.isEmpty()) {
    this->display->drawString(
        logo_width + 5, 5,
        "                    ");  // http://255.255.255.255
  } else {
    this->display->drawString(0, logo_height + 5, "http://" + ipAddress);
  }
  this->display->display();
}

Display::~Display() {
  if (display != NULL) {
    delete display;
  }
}