#include "match.hpp"

std::optional<bool> Match::submit_move(std::string lan)
{
    auto move = Move::create(lan);

    if (!move.has_value()) {
        return std::nullopt;
    }

    if (m_state.size() == 0) {
        return std::nullopt;
    }

    auto current = m_state.at(head);

    auto next = current.second.submit_move(move.value());

    if (!next.has_value()) {
        return false;
    }

    if (head + 1 == m_state.size()) {
        m_state.push_back(std::make_pair(move.value(), next.value()));
    }
    else {
        m_state.at(head) = std::make_pair(move.value(), next.value());
    }

    head += 1;

    return true;
}

Match Match::create()
{
    std::vector<std::pair<Move, MatchState>> state { std::make_pair(Move::nullmove, MatchState::create(constants::starting_fen).value()) };

    return Match(state);
}
