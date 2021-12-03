#ifndef Chip_h
#define Chip_h

enum ChipType {
    TYPE_SX1278 = 0,
    TYPE_SX1273
};

class Chip {
    public:
        Chip(const char *name, ChipType type);
        const char *getName();
        ChipType getType();

    private:
        const char *name;
        ChipType type;
};

#endif
