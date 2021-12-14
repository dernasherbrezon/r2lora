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

void drawFrame1(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x,
                int16_t y) {
  display->drawXbm(x + 10, y, logo_width, logo_height, logo_bits);
  display->setFont(ArialMT_Plain_10);
  display->drawString(x + 10 + logo_width, y + 10, "r2lora");
  display->drawString(x + 10 + logo_width, y + 30,
                      "ip: " + WiFi.localIP().toString());
}
void drawFrame2(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x,
                int16_t y) {
  display->drawString(x + 10 + logo_width, y + 10, "all good");
}

void Display::init() {
  if (PIN_OLED_SDA == 0 || PIN_OLED_SCL == 0) {
    return;
  }
  this->display = new SSD1306Wire(0x3c, PIN_OLED_SDA, PIN_OLED_SCL);
  this->ui = new OLEDDisplayUi(this->display);
  this->ui->setActiveSymbol(activeSymbol);
  this->ui->setInactiveSymbol(inactiveSymbol);
  this->ui->setIndicatorPosition(BOTTOM);
  this->ui->setIndicatorDirection(LEFT_RIGHT);
  this->ui->enableAutoTransition();
  this->ui->setFrameAnimation(SLIDE_LEFT);
  FrameCallback frames[] = {drawFrame1, drawFrame2};
  this->ui->setFrames(frames, 2);
  this->ui->init();
}

void Display::loop() {
  if (this->ui == NULL) {
    return;
  }
  int16_t millisToWaitBeforeNextUpdate = this->ui->update();
  if (millisToWaitBeforeNextUpdate > 0) {
    delay(millisToWaitBeforeNextUpdate);
  }
}

Display::~Display() {
  if (display != NULL) {
    delete display;
  }
  if (ui != NULL) {
    delete ui;
  }
}