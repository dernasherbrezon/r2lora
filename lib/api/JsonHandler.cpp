#include "JsonHandler.h"

JsonHandler::JsonHandler(std::function<int(String &, String *)> func, const Uri &uri, HTTPMethod method, Configurator *config) {
  this->func = func;
  this->uri = uri.clone();
  this->method = method;
  this->config = config;
  this->uri->initPathArgs(pathArgs);
}

JsonHandler::~JsonHandler() {
  delete uri;
}

bool JsonHandler::canHandle(HTTPMethod requestMethod, String requestUri) {
  if (method != HTTP_ANY && method != requestMethod) {
    return false;
  }
  return uri->canHandle(requestUri, pathArgs);
}

bool JsonHandler::canUpload(String requestUri) {
  return false;
}

bool JsonHandler::handle(WebServer &server, HTTPMethod requestMethod, String requestUri) {
  if (!canHandle(requestMethod, requestUri)) {
    return false;
  }
  if (!server.authenticate(config->getUsername(), config->getPassword())) {
    server.requestAuthentication();
    return true;
  }
  String response;
  String body = server.arg("plain");
  int code = this->func(body, &response);
  server.send(code, "application/json; charset=UTF-8", response);
  return true;
}

void JsonHandler::upload(WebServer &server, String requestUri, HTTPUpload &upload) {
  // do nothing
}
