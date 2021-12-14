#include <ApiHandler.h>
#include <ArduinoJson.h>
#include <unity.h>

#include "LoRaFrame.h"
#include "MockLoRaModule.h"

MockLoRaModule mock;
WebServer web(80);

String VALID_RX_REQUEST =
    "{\"freq\":433.0,\"bw\":10.4,\"sf\":9,\"cr\":6,\"syncWord\":18,\"power\":"
    "10,\"preambleLength\":55,\"gain\":0,\"ldro\":0}";

String INVALID_RX_REQUEST =
    "{\"freq\":433.0 \"bw\":10.4,\"sf\":9,\"cr\":6,\"syncWord\":18,\"power\":"
    "10,\"preambleLength\":55,\"gain\":0,\"ldro\":0}";

String VALID_TX_REQUEST = "{\"data\":\"CAFE\",\"power\":10}";
String INVALID_TX_REQUEST = "{\"data\":\"CAFE\" \"power\":10}";
String INVALID_TX_DATA_REQUEST = "{\"data\":\"CAXE\",\"power\":10}";

uint8_t data[] = {0xca, 0xfe};
LoRaFrame frame;

void setUp(void) {
  frame.setData(data);
  frame.setDataLength(sizeof(data) / sizeof(uint8_t));
  frame.setFrequencyError(13.2);
  frame.setRssi(-11.22);
  frame.setSnr(3.2);
  frame.setTimestamp(1605980902);
  mock.expectedFrames.clear();
  mock.expectedFrames.push_back(&frame);
}

void assertStatus(String *actual, const char *status) {
  StaticJsonDocument<128> json;
  DeserializationError error = deserializeJson(json, *actual);
  TEST_ASSERT_NULL(error);
  TEST_ASSERT_EQUAL_STRING(status, json["status"]);
}

void test_success_start(void) {
  ApiHandler handler(&web, &mock, NULL, NULL);
  String output;
  int code = handler.handleStart(VALID_RX_REQUEST, &output);
  TEST_ASSERT_EQUAL_INT(200, code);
  assertStatus(&output, "SUCCESS");
}

void test_no_request(void) {
  ApiHandler handler(&web, &mock, NULL, NULL);
  String output;
  int code = handler.handleStart((String)NULL, &output);
  TEST_ASSERT_EQUAL_INT(200, code);
  assertStatus(&output, "FAILURE");
}

void test_invalid_json(void) {
  ApiHandler handler(&web, &mock, NULL, NULL);
  String output;
  int code = handler.handleStart(INVALID_RX_REQUEST, &output);
  TEST_ASSERT_EQUAL_INT(200, code);
  assertStatus(&output, "FAILURE");
}

void test_begin_failed(void) {
  ApiHandler handler(&web, &mock, NULL, NULL);
  mock.beginCode = -1;
  String output;
  int code = handler.handleStart(VALID_RX_REQUEST, &output);
  TEST_ASSERT_EQUAL_INT(200, code);
  assertStatus(&output, "FAILURE");
}

void test_success_stop_even_if_not_running(void) {
  ApiHandler handler(&web, &mock, NULL, NULL);
  String output;
  int code = handler.handleStop("", &output);
  TEST_ASSERT_EQUAL_INT(200, code);
  assertStatus(&output, "SUCCESS");
}

void test_pull(void) {
  ApiHandler handler(&web, &mock, NULL, NULL);
  String output;
  handler.handleStart(VALID_RX_REQUEST, &output);
  // ignore start response. should be tested in other test cases
  output.clear();
  handler.loop();
  int code = handler.handlePull("", &output);
  TEST_ASSERT_EQUAL_INT(200, code);
  StaticJsonDocument<128> json;
  DeserializationError error = deserializeJson(json, output);
  TEST_ASSERT_NULL(error);
  TEST_ASSERT_EQUAL_STRING("SUCCESS", json["status"]);
  JsonArray frames = json["frames"];
  TEST_ASSERT_EQUAL_INT(1, frames.size());
  JsonObject frame = frames[0];
  TEST_ASSERT_EQUAL_STRING("CAFE", frame["data"]);
  TEST_ASSERT_EQUAL_FLOAT(-11.22, frame["rssi"]);
  TEST_ASSERT_EQUAL_FLOAT(3.2, frame["snr"]);
  TEST_ASSERT_EQUAL_FLOAT(13.2, frame["frequencyError"]);
  TEST_ASSERT_EQUAL_UINT32(1605980902, frame["timestamp"]);
}

void test_frames_after_stop(void) {
  ApiHandler handler(&web, &mock, NULL, NULL);
  String output;
  handler.handleStart(VALID_RX_REQUEST, &output);
  // ignore start response. should be tested in other test cases
  output.clear();
  handler.loop();
  int code = handler.handleStop("", &output);
  TEST_ASSERT_EQUAL_INT(200, code);
  StaticJsonDocument<128> json;
  DeserializationError error = deserializeJson(json, output);
  TEST_ASSERT_NULL(error);
  TEST_ASSERT_EQUAL_STRING("SUCCESS", json["status"]);
  JsonArray frames = json["frames"];
  TEST_ASSERT_EQUAL_INT(1, frames.size());
}

void test_cant_tx_during_receive(void) {
  ApiHandler handler(&web, &mock, NULL, NULL);
  mock.receiving = true;
  String output;
  int code = handler.handleTx(VALID_TX_REQUEST, &output);
  TEST_ASSERT_EQUAL_INT(200, code);
  assertStatus(&output, "FAILURE");
}

void test_invalid_tx_request(void) {
  ApiHandler handler(&web, &mock, NULL, NULL);
  mock.receiving = false;
  String output;
  int code = handler.handleTx(INVALID_TX_REQUEST, &output);
  TEST_ASSERT_EQUAL_INT(200, code);
  assertStatus(&output, "FAILURE");
}

void test_empty_tx_request(void) {
  ApiHandler handler(&web, &mock, NULL, NULL);
  mock.receiving = false;
  String output;
  int code = handler.handleTx("", &output);
  TEST_ASSERT_EQUAL_INT(200, code);
  assertStatus(&output, "FAILURE");
}

void test_invalid_lora_tx_code(void) {
  ApiHandler handler(&web, &mock, NULL, NULL);
  mock.receiving = false;
  mock.txCode = -1;
  String output;
  int code = handler.handleTx(VALID_TX_REQUEST, &output);
  TEST_ASSERT_EQUAL_INT(200, code);
  assertStatus(&output, "FAILURE");
}

void test_invalid_tx_data_request(void) {
  ApiHandler handler(&web, &mock, NULL, NULL);
  mock.receiving = false;
  String output;
  int code = handler.handleTx(INVALID_TX_DATA_REQUEST, &output);
  TEST_ASSERT_EQUAL_INT(200, code);
  assertStatus(&output, "FAILURE");
}

void test_success_tx(void) {
  ApiHandler handler(&web, &mock, NULL, NULL);
  mock.receiving = false;
  String output;
  int code = handler.handleTx(VALID_TX_REQUEST, &output);
  TEST_ASSERT_EQUAL_INT(200, code);
  assertStatus(&output, "SUCCESS");
}

void setup() {
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  delay(2000);

  UNITY_BEGIN();
  RUN_TEST(test_success_start);
  RUN_TEST(test_no_request);
  RUN_TEST(test_invalid_json);
  RUN_TEST(test_begin_failed);
  RUN_TEST(test_success_stop_even_if_not_running);
  RUN_TEST(test_pull);
  RUN_TEST(test_frames_after_stop);
  RUN_TEST(test_cant_tx_during_receive);
  RUN_TEST(test_invalid_tx_request);
  RUN_TEST(test_empty_tx_request);
  RUN_TEST(test_invalid_lora_tx_code);
  RUN_TEST(test_invalid_tx_data_request);
  RUN_TEST(test_success_tx);
  UNITY_END();
}

void loop() { delay(5000); }