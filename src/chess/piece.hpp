#ifndef PIECE_HPP
#define PIECE_HPP
#include <string>
#include "utils.hpp"

class Piece {
public:
    Piece(): m_type(PieceType::None), m_team(Team::None) {}
    Piece(PieceType type, Team team): m_type(type), m_team(team) {}
    Piece(char piece);

    const PieceType type() const
    {
        return m_type;
    }
    const Team team() const
    {
        return m_team;
    }

    std::string to_string() const;
private:
    PieceType m_type;
    Team m_team;
};
#endif
