#include "piece.hpp"

Piece::Piece()
{
    type = PieceType::None;
    team = Team::White;
}

Piece::Piece(PieceType type, Team team)
{
    this->type = type;
    this->team = team;
}
