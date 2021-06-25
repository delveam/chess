#ifndef PIECE_HPP
#define PIECE_HPP
#include <string>
#include "utils.hpp"

class Piece {
public:
    Piece();
    Piece(PieceType type, Team team);
    Piece(char piece);
    PieceType type;
    Team team;
    std::string to_string() const;
};
#endif
