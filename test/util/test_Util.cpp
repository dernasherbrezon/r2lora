#include <Util.h>
#include <unity.h>

uint8_t *output = NULL;
size_t output_len = 0;

void test_success(void) {
  const char *input = "cafe";
  int code = convertStringToHex(input, &output, &output_len);
  TEST_ASSERT_EQUAL_INT(0, code);
  uint8_t expected[] = {0xca, 0xfe};
  size_t expected_len = sizeof(expected) / sizeof(uint8_t);
  TEST_ASSERT_EQUAL_UINT64(expected_len, output_len);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, output, expected_len);
}

void process() {
  UNITY_BEGIN();
  RUN_TEST(test_success);
  UNITY_END();
}

#ifdef ARDUINO
#include <Arduino.h>
void setup() {
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  delay(2000);

  process();
}
void loop() {
  digitalWrite(13, HIGH);
  delay(100);
  digitalWrite(13, LOW);
  delay(500);
}
#else
int main(int argc, char **argv) {
  process();
  return 0;
}
#endif