#ifndef GM_HPP
#define GM_HPP
#include "board.hpp"
#include "move.hpp"

typedef std::set<std::string> MoveSet;
typedef std::map<unsigned int, MoveSet> Moves;
typedef std::array<bool, constants::board_size> DangerZone;

namespace gm {
class Analysis;
enum class KingSafety {
    None,
    Safe,
    Check,
    Checkmate
};

std::optional<Board> apply_move(const Board& board, Move team);
std::optional<Analysis> analyze(const Board& board, Team team);
}

class gm::Analysis {
public:
    Analysis(Moves moves, DangerZone danger_zone, Coordinates king_location, KingSafety king_safety) :
        m_moves(moves),
        m_danger_zone(danger_zone),
        m_king_location(king_location),
        m_king_safety(king_safety)
    {
    }

    const Moves& moves() const
    {
        return m_moves;
    }
    const DangerZone& danger_zone() const
    {
        return m_danger_zone;
    }
    Coordinates king_location() const
    {
        return m_king_location;
    }
    KingSafety king_safety() const
    {
        return m_king_safety;
    }
private:
    Moves m_moves;
    DangerZone m_danger_zone;
    Coordinates m_king_location;
    KingSafety m_king_safety { KingSafety::None };
};
#endif
