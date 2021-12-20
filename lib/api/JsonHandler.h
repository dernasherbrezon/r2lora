#ifndef JsonHandler_h
#define JsonHandler_h

#include <WebServer.h>

class JsonHandler : public RequestHandler {
 public:
  JsonHandler(std::function<int(String &, String *)> func, const Uri &uri, HTTPMethod method, const char *username, const char *password);

  ~JsonHandler();

  bool canHandle(HTTPMethod requestMethod, String requestUri) override;

  bool canUpload(String requestUri) override;

  bool handle(WebServer &server, HTTPMethod requestMethod, String requestUri) override;

  void upload(WebServer &server, String requestUri, HTTPUpload &upload) override;

 private:
  std::function<int(String&, String *)> func = nullptr;
  Uri *uri;
  HTTPMethod method;
  const char *apiUsername = NULL;
  const char *apiPassword = NULL;
};

#endif
