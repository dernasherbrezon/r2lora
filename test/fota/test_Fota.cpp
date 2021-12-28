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

#ifndef ARDUINO_VARIANT
#define ARDUINO_VARIANT "native"
#endif

const char *hostname = "apt.r2server.ru";

void test_invalid_hostname() {
  Fota fota;
  fota.init("1.0", "255.255.255.255", 80, "/fotatest/success.json", 24 * 60 * 60 * 1000, ARDUINO_VARIANT);
  TEST_ASSERT_EQUAL(FOTA_UNKNOWN_ERROR, fota.loop(false));
}

void test_unknown_indexfile() {
  Fota fota;
  fota.init("1.0", hostname, 80, "/fotatest/nonexistingfile.json", 24 * 60 * 60 * 1000, ARDUINO_VARIANT);
  TEST_ASSERT_EQUAL(FOTA_NO_UPDATES, fota.loop(false));
}

void test_no_new_version() {
  Fota fota;
  fota.init("1.1", hostname, 80, "/fotatest/success.json", 24 * 60 * 60 * 1000, ARDUINO_VARIANT);
  TEST_ASSERT_EQUAL(FOTA_NO_UPDATES, fota.loop(false));
}

void test_invalid_json() {
  Fota fota;
  fota.init("1.0", hostname, 80, "/fotatest/invalidjson.json", 24 * 60 * 60 * 1000, ARDUINO_VARIANT);
  TEST_ASSERT_EQUAL(FOTA_INVALID_SERVER_RESPONSE, fota.loop(false));
}

void test_non_existing_file() {
  Fota fota;
  fota.init("1.0", hostname, 80, "/fotatest/missingfile.json", 24 * 60 * 60 * 1000, ARDUINO_VARIANT);
  TEST_ASSERT_EQUAL(FOTA_INVALID_SERVER_RESPONSE, fota.loop(false));
}

void test_no_checksum_field() {
  Fota fota;
  fota.init("1.0", hostname, 80, "/fotatest/nochecksum.json", 24 * 60 * 60 * 1000, ARDUINO_VARIANT);
  TEST_ASSERT_EQUAL(FOTA_INVALID_SERVER_RESPONSE, fota.loop(false));
}

void test_invalid_checksum() {
  Fota fota;
  fota.init("1.0", hostname, 80, "/fotatest/invalidchecksum.json", 24 * 60 * 60 * 1000, ARDUINO_VARIANT);
  TEST_ASSERT_EQUAL(FOTA_INVALID_SERVER_RESPONSE, fota.loop(false));
}

void test_success() {
  Fota fota;
  fota.init("1.0", hostname, 80, "/fotatest/success.json", 24 * 60 * 60 * 1000, ARDUINO_VARIANT);
  fota.setOnUpdate([](size_t cur, size_t total) {
    log_i("progress: %d/%d", cur, total);
  });
  TEST_ASSERT_EQUAL(FOTA_SUCCESS, fota.loop(false));
  TEST_ASSERT_EQUAL(FOTA_NO_UPDATES, fota.loop(false));
}

void test_not_initialized() {
  Fota fota;
  TEST_ASSERT_EQUAL(FOTA_NO_UPDATES, fota.loop(false));
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
  RUN_TEST(test_no_checksum_field);
  RUN_TEST(test_invalid_checksum);
  RUN_TEST(test_success);
  RUN_TEST(test_not_initialized);
  UNITY_END();
}

void loop() { delay(5000); }