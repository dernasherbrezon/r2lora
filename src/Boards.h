#ifndef Boards_h
#define Boards_h

#define STRING_LEN 256

#include "Board.h"

class Boards {
 public:
  Boards();
  std::vector<Board> getAll();
  char *getBoardNames();
  char *getBoardIndices();

 private:
  std::vector<Board> boards;
  char *board_names;
  char *board_indices;
};
#endif
