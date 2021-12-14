#include <ApiHandler.h>
#include <ArduinoJson.h>
#include <unity.h>

#include "MockLoRaModule.h"

MockLoRaModule mock;
WebServer web(80);

String VALID_RX_REQUEST =
    "{\"freq\":433.0,\"bw\":10.4,\"sf\":9,\"cr\":6,\"syncWord\":18,\"power\":"
    "10,\"preambleLength\":55,\"gain\":0,\"ldro\":0}";

String INVALID_RX_REQUEST =
    "{\"freq\":433.0 \"bw\":10.4,\"sf\":9,\"cr\":6,\"syncWord\":18,\"power\":"
    "10,\"preambleLength\":55,\"gain\":0,\"ldro\":0}";

void assertStatus(String *actual, const char *status) {
  StaticJsonDocument<128> json;
  DeserializationError error = deserializeJson(json, *actual);
  TEST_ASSERT_NULL(error);
  TEST_ASSERT_EQUAL_STRING(status, json["status"]);
}

void test_success_start(void) {
  ApiHandler handler(&web, &mock, NULL, NULL);
  mock.beginCode = 0;
  String output;
  int code = handler.handleStart(VALID_RX_REQUEST, &output);
  TEST_ASSERT_EQUAL_INT(200, code);
  assertStatus(&output, "SUCCESS");
}

void test_no_request(void) {
  ApiHandler handler(&web, &mock, NULL, NULL);
  mock.beginCode = 0;
  String output;
  int code = handler.handleStart((String)NULL, &output);
  TEST_ASSERT_EQUAL_INT(200, code);
  assertStatus(&output, "FAILURE");
}

void test_invalid_json(void) {
  ApiHandler handler(&web, &mock, NULL, NULL);
  mock.beginCode = 0;
  String output;
  int code = handler.handleStart(INVALID_RX_REQUEST, &output);
  TEST_ASSERT_EQUAL_INT(200, code);
  assertStatus(&output, "FAILURE");
}

void setup() {
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  delay(2000);

  UNITY_BEGIN();
  RUN_TEST(test_success_start);
  RUN_TEST(test_no_request);
  RUN_TEST(test_invalid_json);
  UNITY_END();
}

void loop() { delay(5000); }