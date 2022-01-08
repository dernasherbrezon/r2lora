#ifndef Chip_h
#define Chip_h

enum ChipType {
    TYPE_SX1278 = 0,
    TYPE_SX1276
};

class Chip {
    public:
        Chip(const char *name, ChipType type);
        const char *getName();
        ChipType getType();

        bool loraSupported = false;
        float minLoraFrequency = 0.0;
        float maxLoraFrequency = 0.0;

        bool fskSupported = false;
        float minFskFrequency = 0.0;
        float maxFskFrequency = 0.0;

    private:
        const char *name;
        ChipType type;
};

#endif
