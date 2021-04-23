#include "piece.hpp"

Piece::Piece()
{
    type = utils::ChessPiece::None;
    team = utils::Player::White;
}

Piece::Piece(utils::ChessPiece type, utils::Player team)
{
    this->type = type;
    this->team = team;
}
