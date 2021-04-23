#ifndef BOARD_HPP
#define BOARD_HPP
#include <array>
#include <string>
#include "piece.hpp"

class Board {
public:
    Board();
    std::array<Piece, 64> pieces;
    static Board load_from_fen(std::string fen);
};

#endif
