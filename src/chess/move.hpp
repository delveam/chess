#ifndef MOVE_HPP
#define MOVE_HPP
#include "coordinates.hpp"
#include "utils.hpp"

class Move {
public:
    std::string lan() const
    {
        return m_lan;
    }
    Coordinates start() const
    {
        return m_start;
    }
    Coordinates end() const
    {
        return m_end;
    }
    std::optional<PieceType> promotion() const
    {
        return m_promotion;
    }

    static std::optional<Move> create(std::string lan);
private:
    Move() = default;
    Move(std::string lan, Coordinates start, Coordinates end, std::optional<PieceType> promotion) :
        m_lan(lan),
        m_start(start),
        m_end(end),
        m_promotion(promotion)
    {
    }

    std::string m_lan;
    Coordinates m_start;
    Coordinates m_end;
    std::optional<PieceType> m_promotion;
};

#endif
