#ifndef LoRaModule_h
#define LoRaModule_h

#include "Board.h"
#include "ObservationRequest.h"
#include "LoRaFrame.h"

class LoRaModule {
    public:
        void setup(Board board);
        int begin(ObservationRequest *req);
        LoRaFrame *loop();
    private:
        
};

#endif