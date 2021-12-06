#define LoRaModule_h
#include <Chip.h>
#include <ObservationRequest.h>

class LoRaModule {
 public:
  ~LoRaModule() {}
  int setup(Chip chip) { return 0; }
  int begin(ObservationRequest *req) { return 0; }
  LoRaFrame *loop() { return NULL; }
  void end() {}
  bool isReceivingData() { return false; }
};

#include <ObservationHandler.h>
#include <unity.h>

void test_init_loop(void) {
  LoRaModule mock;
  WebServer web(80);

  ObservationHandler handler(&web, &mock);
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