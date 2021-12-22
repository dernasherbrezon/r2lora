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
  fota.init("1.0", "255.255.255.255", 80, "/fotatest/success.json", 24 * 60 * 60 * 1000, BOARD_NAME);
  TEST_ASSERT_EQUAL(FOTA_UNKNOWN_ERROR, fota.loop());
}

void test_unknown_indexfile() {
  Fota fota;
  fota.init("1.0", "http://apt.r2server.ru", 80, "/fotatest/nonexistingfile.json", 24 * 60 * 60 * 1000, BOARD_NAME);
  TEST_ASSERT_EQUAL(FOTA_NO_UPDATES, fota.loop());
}

void test_no_new_version() {
  Fota fota;
  fota.init("1.1", "http://apt.r2server.ru", 80, "/fotatest/success.json", 24 * 60 * 60 * 1000, BOARD_NAME);
  TEST_ASSERT_EQUAL(FOTA_NO_UPDATES, fota.loop());
}

void test_invalid_json() {
  Fota fota;
  fota.init("1.1", "http://apt.r2server.ru", 80, "/fotatest/invalidjson.json", 24 * 60 * 60 * 1000, BOARD_NAME);
  TEST_ASSERT_EQUAL(FOTA_INVALID_SERVER_RESPONSE, fota.loop());
}

void test_non_existing_file() {
  Fota fota;
  fota.init("1.1", "http://apt.r2server.ru", 80, "/fotatest/missingfile.json", 24 * 60 * 60 * 1000, BOARD_NAME);
  TEST_ASSERT_EQUAL(FOTA_INVALID_SERVER_RESPONSE, fota.loop());
}

void setup() {
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  delay(2000);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  log_i("connected: %s", WiFi.localIP().toString().c_str());

  UNITY_BEGIN();
  RUN_TEST(test_invalid_hostname);
  RUN_TEST(test_unknown_indexfile);
  RUN_TEST(test_no_new_version);
  RUN_TEST(test_invalid_json);
  RUN_TEST(test_non_existing_file);
  UNITY_END();
}

void loop() { delay(5000); }