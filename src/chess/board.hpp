#ifndef BOARD_HPP
#define BOARD_HPP
#include <array>
#include "piece.hpp"

class Board {
public:
    Board();
    std::array<Piece, 64> pieces;
};

#endif
