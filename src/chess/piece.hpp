#ifndef PIECE_HPP
#define PIECE_HPP
#include <string>
#include "utils.hpp"

class Piece {
public:
    Piece();
    Piece(PieceType type, Team team);
    Piece(char piece);
    std::string to_string();
    PieceType type;
    Team team;
};
#endif
