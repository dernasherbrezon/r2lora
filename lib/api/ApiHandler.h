#ifndef ApiHandler_h
#define ApiHandler_h

#include <LoRaFrame.h>
#include <LoRaModule.h>
#include <WebServer.h>

class ApiHandler {
 public:
  ApiHandler(WebServer *web, LoRaModule *lora, const char *apiUsername,
             const char *apiPassword);
  void loop();

  // for tests mostly
  int handleStart(String body, String *output);
  int handleStop(String body, String *output);
  int handlePull(String body, String *output);
  int handleTx(String body, String *output);

 private:

  void sendFailure(const char *message, String *output);
  void sendSuccess(String *output);

  WebServer *web = NULL;
  LoRaModule *lora = NULL;
  bool receiving = false;
  std::vector<LoRaFrame *> receivedFrames;
};

#endif
