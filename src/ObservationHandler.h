#ifndef ObservationHandler_h
#define ObservationHandler_h

#include <WebServer.h>
#include "LoRaModule.h"

class ObservationHandler {
    public:
        ObservationHandler(WebServer *web, LoRaModule *lora);
        void loop();
    private:
        void handleStart();
        void handleStop();
        void handlePull();
        
        WebServer *web;
        LoRaModule *lora;
        bool receiving;
        std::vector<LoRaFrame *> receivedFrames;
};

#endif
