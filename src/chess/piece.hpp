#ifndef PIECE_HPP
#define PIECE_HPP
#include "utils.hpp"
struct Piece {
public:
    utils::ChessPiece type;
    utils::Player team;
};
#endif
