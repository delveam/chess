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

Piece::Piece(char piece)
{
    switch (piece) {
    // White
    case 'P':
        type = PieceType::Pawn;
        team = Team::White;
        break;
    case 'N':
        type = PieceType::Knight;
        team = Team::White;
        break;
    case 'B':
        type = PieceType::Bishop;
        team = Team::White;
        break;
    case 'R':
        type = PieceType::Rook;
        team = Team::White;
        break;
    case 'Q':
        type = PieceType::Queen;
        team = Team::White;
        break;
    case 'K':
        type = PieceType::King;
        team = Team::White;
        break;

    // Black
    case 'p':
        type = PieceType::Pawn;
        team = Team::Black;
        break;
    case 'n':
        type = PieceType::Knight;
        team = Team::Black;
        break;
    case 'b':
        type = PieceType::Bishop;
        team = Team::Black;
        break;
    case 'r':
        type = PieceType::Rook;
        team = Team::Black;
        break;
    case 'q':
        type = PieceType::Queen;
        team = Team::Black;
        break;
    case 'k':
        type = PieceType::King;
        team = Team::Black;
        break;


    default:
        type = PieceType::None;
        team = Team::White;
        break;
    }
}
