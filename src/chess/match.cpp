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

    // If the head is not the end then we need to create a new timeline.
    if (m_head != m_end) {
        m_head += 1;
        m_end = m_head;
        m_state.at(m_head) = next;

        return true;
    }

    // Otherwise, we can just add to the end of the list.
    m_state.push_back(next);
    m_head += 1;
    m_end += 1;

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
