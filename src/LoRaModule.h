#ifndef LoRaModule_h
#define LoRaModule_h

#include "Board.h"
#include "ObservationRequest.h"
#include "LoRaFrame.h"
#include "Chip.h"

class LoRaModule {
    public:
        int setup(Board board, Chip chip);
        int begin(ObservationRequest *req);
        LoRaFrame *loop();
    private:
        PhysicalLayer *phys;
        ChipType type;
        
};

#endif