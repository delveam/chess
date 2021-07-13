#include "analysis.hpp"
#include "move.hpp"

bool gm::Analysis::contains_move(std::string lan) const
{
    auto move = Move::create(lan);

    if (!move.has_value()) {
        return false;
    }

    auto coords = Coordinates::from_string(lan.substr(0, 2));

    if (!coords.has_value()) {
        return false;
    }

    auto index = coords->y() * constants::board_width + coords->x();

    if (m_moves.count(index) == 0) {
        return false;
    }

    for (const auto& entry : m_moves.at(index)) {
        if (lan == entry) {
            return true;
        }
    }

    return false;
}
