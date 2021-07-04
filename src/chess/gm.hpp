#ifndef GM_HPP
#define GM_HPP
#include "board.hpp"
#include "move.hpp"

typedef std::set<std::string> MoveSet;
typedef std::map<unsigned int, MoveSet> Moves;
typedef std::array<bool, constants::board_size> DangerZone;

namespace gm {
class Analysis;
enum class Event {
    None,
    Check,
    Checkmate
};

std::optional<Board> apply_move(const Board& board, Move team);
DangerZone generate_danger_zone(const Board& board, Team team);
std::optional<Moves> generate_moves(const Board& board, Team team);
}

class gm::Analysis {
public:
    Analysis() = default;
    Analysis(Moves moves, DangerZone danger_zone, Event event) :
        m_moves(moves),
        m_danger_zone(danger_zone),
        m_event(event)
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
    Event event() const
    {
        return m_event;
    }
private:
    Moves m_moves;
    DangerZone m_danger_zone;
    Event m_event { Event::None };
};
#endif
