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

    auto current = m_state.at(m_head);

    auto board = current.second.submit_move(move.value());

    if (!board.has_value()) {
        return false;
    }

    auto next = std::make_pair(move.value(), board.value());

    m_head += 1;
    // Note that the following is required to prevent the user from redoing into an invalid state.
    m_end = m_head;

    // If we are not at the end of the vector then replace whatever is at the current index.
    if (m_head != m_state.size()) {
        m_state.at(m_head) = next;

        return true;
    }

    // Otherwise, we can just add to the end of the vector.
    m_state.push_back(next);

    return true;
}

void Match::undo()
{
    if (m_head == 0) {
        return;
    }

    m_head -= 1;
}

void Match::redo()
{
    if (m_head == m_end) {
        return;
    }

    m_head += 1;
}

Match Match::create()
{
    std::vector<std::pair<Move, MatchState>> state { std::make_pair(Move::nullmove, MatchState::create(constants::starting_fen).value()) };

    return Match(state);
}
