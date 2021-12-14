#include <ApiHandler.h>
#include <unity.h>
#include "MockLoRaModule.h"

String VALID_RX_REQUEST = String("{\n")
	+ "	\"freq\": 433.0,\n"
	+ "	\"bw\": 10.4,\n"
	+ "	\"sf\": 9,\n"
	+ "	\"cr\": 6,\n"
	+ "	\"syncWord\": 18,\n"
	+ "	\"preambleLength\": 55,\n"
	+ "	\"gain\": 0,\n"
	+ "	\"ldro\": 0\n"
	+ "}\n";

void test_success_start(void) {
  MockLoRaModule mock;
  WebServer web(80);

  ApiHandler handler(&web, &mock, NULL, NULL);
  mock.beginCode = 0;
  String output;
  int code = handler.handleStart(VALID_RX_REQUEST, &output);
  TEST_ASSERT_EQUAL_INT(200, code);
}

void setup() {
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  delay(2000);

  UNITY_BEGIN();
  RUN_TEST(test_success_start);
  UNITY_END();
}

void loop() { delay(5000); }