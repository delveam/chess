#ifndef PIECE_HPP
#define PIECE_HPP
#include "utils.hpp"

class Piece {
public:
    Piece();
    Piece(PieceType type, Team team);
    PieceType type;
    Team team;
};
#endif
