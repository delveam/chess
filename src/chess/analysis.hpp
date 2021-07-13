#ifndef ANALYSIS_HPP
#define ANALYSIS_HPP
#include <map>
#include <set>
#include <string>
#include "coordinates.hpp"
#include "king_safety.hpp"
#include "utils.hpp"

typedef std::set<std::string> MoveSet;
typedef std::map<unsigned int, MoveSet> Moves;
typedef std::array<bool, constants::board_size> DangerZone;

namespace gm {
class Analysis;
};

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

    bool contains_move(std::string lan) const;
private:
    Moves m_moves;
    DangerZone m_danger_zone;
    Coordinates m_king_location;
    KingSafety m_king_safety { KingSafety::None };
};
#endif
