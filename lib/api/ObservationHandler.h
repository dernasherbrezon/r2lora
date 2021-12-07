#ifndef ObservationHandler_h
#define ObservationHandler_h

#include <LoRaFrame.h>
#include <LoRaModule.h>
#include <WebServer.h>

class ObservationHandler {
 public:
  ObservationHandler(WebServer *web, LoRaModule *lora, const char *apiUsername,
                     const char *apiPassword);
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
  const char *apiUsername = NULL;
  const char *apiPassword = NULL;
  std::vector<LoRaFrame *> receivedFrames;
};

#endif
