#ifndef BOARD_HPP
#define BOARD_HPP
#include <array>
#include <optional>
#include <string>
#include "piece.hpp"

class Board {
public:
    Board();
    std::array<Piece, BOARD_WIDTH * BOARD_HEIGHT> pieces;
    std::optional<Piece> get(unsigned int x, unsigned int y);
    void move_uci(std::string notation);
    static Board load_from_fen(std::string fen);
};

#endif
