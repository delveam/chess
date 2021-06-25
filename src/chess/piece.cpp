#include <optional>
#include "piece.hpp"

Piece::Piece(char piece)
{
    switch (piece) {
    // White
    case 'P':
        m_type = PieceType::Pawn;
        m_team = Team::White;
        break;
    case 'N':
        m_type = PieceType::Knight;
        m_team = Team::White;
        break;
    case 'B':
        m_type = PieceType::Bishop;
        m_team = Team::White;
        break;
    case 'R':
        m_type = PieceType::Rook;
        m_team = Team::White;
        break;
    case 'Q':
        m_type = PieceType::Queen;
        m_team = Team::White;
        break;
    case 'K':
        m_type = PieceType::King;
        m_team = Team::White;
        break;

    // Black
    case 'p':
        m_type = PieceType::Pawn;
        m_team = Team::Black;
        break;
    case 'n':
        m_type = PieceType::Knight;
        m_team = Team::Black;
        break;
    case 'b':
        m_type = PieceType::Bishop;
        m_team = Team::Black;
        break;
    case 'r':
        m_type = PieceType::Rook;
        m_team = Team::Black;
        break;
    case 'q':
        m_type = PieceType::Queen;
        m_team = Team::Black;
        break;
    case 'k':
        m_type = PieceType::King;
        m_team = Team::Black;
        break;

    default:
        m_type = PieceType::None;
        m_team = Team::None;
    }
}

std::string Piece::to_string() const
{
    std::optional<char> temp = std::nullopt;

    switch(m_type) {
    case PieceType::Pawn:
        temp = 'p';
        break;
    case PieceType::Bishop:
        temp = 'b';
        break;
    case PieceType::Knight:
        temp = 'n';
        break;
    case PieceType::Rook:
        temp = 'r';
        break;
    case PieceType::Queen:
        temp = 'q';
        break;
    case PieceType::King:
        temp = 'k';
        break;
    case PieceType::None:
        break;
    }

    if (temp.has_value() && m_team == Team::White) {
        temp = toupper(temp.value());
    }

    std::string result = "";
    if (temp.has_value()) {
        result.push_back(temp.value());
    }

    return result;
}
