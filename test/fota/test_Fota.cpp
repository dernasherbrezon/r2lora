#include <Fota.h>
#include <WiFi.h>
#include <esp32-hal-log.h>
#include <unity.h>

#ifndef WIFI_SSID
#define WIFI_SSID ""
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD ""
#endif

#ifndef BOARD_NAME
#define BOARD_NAME "native"
#endif

void test_invalid_hostname() {
  Fota fota;
  fota.init("1.0", "255.255.255.255", 80, "/fota/r2lora.json", 24 * 60 * 60 * 1000, BOARD_NAME);
  TEST_ASSERT_EQUAL(FOTA_UNKNOWN_ERROR, fota.loop());
}

void setup() {
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  delay(2000);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  log_i("connected: %s", WiFi.localIP());

  UNITY_BEGIN();
  RUN_TEST(test_invalid_hostname);
  UNITY_END();
}

void loop() { delay(5000); }