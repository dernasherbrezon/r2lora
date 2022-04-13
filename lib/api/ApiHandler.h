#ifndef ApiHandler_h
#define ApiHandler_h

#include <ArduinoJson.h>
#include <Configurator.h>
#include <LoRaFrame.h>
#include <LoRaModule.h>
#include <WebServer.h>

class ApiHandler {
 public:
  ApiHandler(WebServer *web, LoRaModule *lora, Configurator *config);
  void loop();

  // public scope for tests mostly
  int handleStart(String &body, String *output);
  int handleStop(String &body, String *output);
  int handlePull(String &body, String *output);
  int handleTx(String &body, String *output);

  int handleFskStart(bool ook, String &body, String *output);
  int handleFskTx(bool ook, String &body, String *output);

 private:
  void sendFailure(const char *status, const char *message, String *output);
  void sendSuccess(String *output);

  int readLoraRequest(const JsonDocument &doc, LoraState *lora);
  int readFskRequest(const JsonDocument &doc, FskState *fsk);

  WebServer *web = NULL;
  LoRaModule *lora = NULL;
  bool receiving = false;
  std::vector<LoRaFrame *> receivedFrames;
};

#endif
