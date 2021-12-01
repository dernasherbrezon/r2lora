#include <stdint.h>
#include <stddef.h>

class LoRaFrame {
    private:
        uint8_t *data;
        size_t data_len;
        float rssi;
        float snr;
};
