#include <WebServer.h>

class JsonRequestHandler: public RequestHandler {
public:

    JsonRequestHandler(const Uri &uri, HTTPMethod method, void (*func)(String json));
    
    bool canHandle(HTTPMethod requestMethod, String requestUri) override  {
        if (_method != HTTP_ANY && _method != requestMethod)
            return false;

        return _uri->canHandle(requestUri, pathArgs);
    }

    bool canUpload(String requestUri) override  {
        if (!_ufn || !canHandle(HTTP_POST, requestUri))
            return false;

        return true;
    }

    bool handle(WebServer& server, HTTPMethod requestMethod, String requestUri) override {
        (void) server;
        if (!canHandle(requestMethod, requestUri))
            return false;

        _fn();
        return true;
    }

    void upload(WebServer& server, String requestUri, HTTPUpload& upload) override {
        (void) server;
        (void) upload;
        if (canUpload(requestUri))
            _ufn();
    }

protected:
    WebServer::THandlerFunction _fn;
    WebServer::THandlerFunction _ufn;
    Uri *_uri;
    HTTPMethod _method;
};