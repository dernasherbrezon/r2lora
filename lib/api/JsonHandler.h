#ifndef JsonHandler_h
#define JsonHandler_h

#include <WebServer.h>
#include <Configurator.h>

class JsonHandler : public RequestHandler {
 public:
  JsonHandler(std::function<int(String &, String *)> func, const Uri &uri, HTTPMethod method, Configurator *config);

  ~JsonHandler();

  bool canHandle(HTTPMethod requestMethod, String requestUri) override;

  bool canUpload(String requestUri) override;

  bool handle(WebServer &server, HTTPMethod requestMethod, String requestUri) override;

  void upload(WebServer &server, String requestUri, HTTPUpload &upload) override;

 private:
  std::function<int(String&, String *)> func = nullptr;
  Uri *uri = NULL;
  HTTPMethod method;
  Configurator *config = NULL;
};

#endif
