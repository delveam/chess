#ifndef PIECE_HPP
#define PIECE_HPP
#include "utils.hpp"

class Piece {
public:
    Piece();
    Piece(PieceType type, Team team);
    Piece(char piece);
    PieceType type;
    Team team;
};
#endif
