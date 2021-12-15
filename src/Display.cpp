#include "Display.h"

#include <WiFi.h>

// defined in platformio.ini
#ifndef PIN_OLED_SDA
#define PIN_OLED_SDA 0
#endif
#ifndef PIN_OLED_SCL
#define PIN_OLED_SCL 0
#endif

Display::Display(LoRaModule *lora) { this->lora = lora; }

void Display::init() {
  if (PIN_OLED_SDA == 0 || PIN_OLED_SCL == 0) {
    return;
  }
  this->display = new SSD1306Wire(0x3c, PIN_OLED_SDA, PIN_OLED_SCL);
  this->display->drawXbm(10, 0, logo_width, logo_height, logo_bits);
  this->display->setFont(ArialMT_Plain_10);
  this->display->drawString(10 + logo_width + 10, 10, "r2lora");
  this->display->drawString(10 + logo_width + 10, 30, "status: UPDATEING");
  this->display->drawString(10, logo_height + 10, "http://255.255.255.255");
  this->display->drawProgressBar(10, logo_height + 30, 120, 10, 45);

  this->display->init();
  this->display->display();
}

void Display::loop() {}

Display::~Display() {
  if (display != NULL) {
    delete display;
  }
}