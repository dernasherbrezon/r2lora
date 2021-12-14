#include <LoRaModule.h>
#include <unity.h>

void test_start_loop_stop(void) {
  LoRaModule module;
  int code = module.setup(Chip("SX1276 - 868/915Mhz", ChipType::TYPE_SX1276));
  TEST_ASSERT_EQUAL(0, code);
  code = module.begin(915.0, 125.0, (uint8_t)9U, (uint8_t)7U, (uint8_t)18U,
                      (uint16_t)8U, (uint8_t)0U, 0);
  TEST_ASSERT_EQUAL(0, code);
  TEST_ASSERT_EQUAL(true, module.isReceivingData());
  // unlikely any message received
  TEST_ASSERT_EQUAL(NULL, module.loop());
  module.end();
}

void setup() {
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  delay(2000);

  UNITY_BEGIN();
  RUN_TEST(test_start_loop_stop);
  UNITY_END();
}

void loop() { delay(5000); }