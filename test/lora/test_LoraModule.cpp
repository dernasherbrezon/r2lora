#include <LoRaModule.h>
#include <unity.h>

void test_start_loop_stop(void) {
  LoRaModule module;
  int code = module.init(&Chip("SX1276 - 868/915Mhz", ChipType::TYPE_SX1276));
  TEST_ASSERT_EQUAL(0, code);
  LoraState req;
  req.freq = 915.0;
  req.bw = 125.0;
  req.sf = (uint8_t)9U;
  req.cr = (uint8_t)7U;
  req.syncWord = (uint8_t)18U;
  req.preambleLength = (uint16_t)8U;
  req.gain = (uint8_t)0U;
  req.ldro = LdroType::LDRO_AUTO;
  code = module.startLoraRx(&req);
  TEST_ASSERT_EQUAL(0, code);
  TEST_ASSERT_EQUAL(true, module.isReceivingData());
  // unlikely any message received
  TEST_ASSERT_EQUAL(NULL, module.loop());
  module.stopRx();
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