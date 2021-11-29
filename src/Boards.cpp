#include "Boards.h"

Boards::Boards() {
    this->boards.push_back(Board("TTGO LORA32 V1.0", 18, 26, 14, 5, 19, 27));
    this->boards.push_back(Board("TTGO LORA32 V2.0", 18, 26, 12, 5, 19, 27));

    for (std::size_t i = 0; i < this->boards.size(); i++) {
        this->board_names.push_back(this->boards[i].getName());

        char *buffer = (char *)malloc(sizeof(char) * STRING_LEN);
        sprintf(buffer, "%d", i);
        this->board_indices.push_back(buffer);
    }
}

char *Boards::getBoardIndices() {
    return (char *)&this->board_indices[0];
}

char *Boards::getBoardNames() {
    return (char *)&this->board_names[0];
}

std::vector<Board> Boards::getAll() {
    return this->boards;
}