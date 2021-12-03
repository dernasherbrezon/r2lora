#include <LoRaModule.h>
#include <unity.h>

void test_start_loop_stop(void) {
  LoRaModule module;
  int code = module.setup(Chip("SX1273 - 868/915Mhz", ChipType::TYPE_SX1273));
  TEST_ASSERT_EQUAL(0, code);
  ObservationRequest req;
  req.setFreq(915.0);
  req.setBw(125.0);
  req.setSf((uint8_t)9U);
  req.setCr((uint8_t)7U);
  req.setSyncWord((uint8_t)18U);
  req.setPower((int8_t)10);
  req.setPreambleLength((uint16_t)8U);
  req.setGain((uint8_t)0U);
  req.setLdro(0);
  code = module.begin(&req);
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

void loop() {
  digitalWrite(13, HIGH);
  delay(100);
  digitalWrite(13, LOW);
  delay(500);
}