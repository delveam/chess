#ifndef PIECE_HPP
#define PIECE_HPP
#include <string>
#include "utils.hpp"

class Piece {
public:
    Piece() = default;
    Piece(PieceType type, Team team) :
        m_type(type),
        m_team(team)
    {
    }
    Piece(char piece);

    PieceType type() const
    {
        return m_type;
    }
    Team team() const
    {
        return m_team;
    }

    std::string to_string() const;
private:
    PieceType m_type { PieceType::None };
    Team m_team { Team::None };
};
#endif
