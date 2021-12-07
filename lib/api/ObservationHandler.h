#ifndef ObservationHandler_h
#define ObservationHandler_h

#include <WebServer.h>
#include <LoRaModule.h>
#include <LoRaFrame.h>

class ObservationHandler {
 public:
  ObservationHandler(WebServer *web, LoRaModule *lora);
  void loop();

 private:
  void handleStart();
  void handleStop();
  void handlePull();

  void sendFailure(const char *message);
  void sendSuccess();

  WebServer *web = NULL;
  LoRaModule *lora = NULL;
  bool receiving = false;
  std::vector<LoRaFrame *> receivedFrames;
};

#endif
