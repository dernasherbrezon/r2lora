#ifndef ObservationHandler_h
#define ObservationHandler_h

#include <WebServer.h>
#include "LoRaModule.h"

class ObservationHandler {
    public:
        ObservationHandler(WebServer *web, LoRaModule *lora);
    private:
        void handleStart();
        WebServer *web;
};

#endif
