#ifndef PIECE_HPP
#define PIECE_HPP
#include "utils.hpp"
class Piece {
public:
    Piece();
    Piece(utils::ChessPiece type, utils::Player team);
    utils::ChessPiece type;
    utils::Player team;
};
#endif
