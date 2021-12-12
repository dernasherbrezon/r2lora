#include <ApiHandler.h>
#include <unity.h>

void test_init_loop(void) {
  LoRaModule mock;
  WebServer web(80);

  ApiHandler handler(&web, &mock, NULL, NULL);
  handler.loop();
}

void setup() {
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  delay(2000);

  UNITY_BEGIN();
  RUN_TEST(test_init_loop);
  UNITY_END();
}

void loop() { delay(5000); }