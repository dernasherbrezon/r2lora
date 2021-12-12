#include <Util.h>
#include <unity.h>

uint8_t *output = NULL;
size_t output_len = 0;

void assertInput(const char *input) {
  int code = convertStringToHex(input, &output, &output_len);
  TEST_ASSERT_EQUAL_INT(0, code);
  uint8_t expected[] = {0xca, 0xfe};
  size_t expected_len = sizeof(expected) / sizeof(uint8_t);
  // there is no size_t comparison in unity.h
  TEST_ASSERT_TRUE(expected_len == output_len);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, output, expected_len);
}

void assertInvalidInput(const char *input) {
  int code = convertStringToHex(input, &output, &output_len);
  TEST_ASSERT_EQUAL_INT(-1, code);
}

void test_success(void) { assertInput("cafe"); }
void test_spaces(void) { assertInput("ca fe"); }
void test_multiple_spaces(void) { assertInput("   ca fe        "); }
void test_case(void) { assertInput("cA Fe"); }
void test_invalid_chars(void) { assertInvalidInput("caxe"); }
void test_invalid_length(void) { assertInvalidInput("caf "); }

void process() {
  UNITY_BEGIN();
  RUN_TEST(test_success);
  UNITY_END();
}

void tearDown(void) {
  if (output != NULL) {
    free(output);
    output = NULL;
  }
  output_len = 0;
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