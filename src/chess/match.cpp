#include "match.hpp"

bool Match::submit_move(std::string lan)
{
    auto move = Move::create(lan);

    if (!move.has_value()) {
        return false;
    }

    if (m_state.size() == 0) {
        return false;
    }

    auto current = m_state.at(m_index);

    auto board = current.second.submit_move(move.value());

    if (!board.has_value()) {
        return false;
    }

    auto next = std::make_pair(move.value(), board.value());

    m_index += 1;
    // Note that the following is required to prevent the user from redoing into an invalid state.
    m_end = m_index;

    // If we are not at the end of the vector then replace whatever is at the current index.
    if (m_index != m_state.size()) {
        m_state.at(m_index) = next;

        return true;
    }

    // Otherwise, we can just add to the end of the vector.
    m_state.push_back(next);

    return true;
}

void Match::undo()
{
    if (m_index == 0) {
        return;
    }

    m_index -= 1;
}

void Match::redo()
{
    if (m_index == m_end) {
        return;
    }

    m_index += 1;
}

Match Match::create()
{
    std::vector<std::pair<Move, MatchState>> state { std::make_pair(Move::nullmove, MatchState::create(constants::starting_fen).value()) };

    return Match(state);
}

std::optional<Match> Match::create_from_position(std::string fen)
{
    auto match_state = MatchState::create(fen);

    if (!match_state.has_value()) {
        return std::nullopt;
    }

    std::vector<std::pair<Move, MatchState>> state { std::make_pair(Move::nullmove, match_state.value()) };

    return Match(state);
}

Match Match::create_from_board(Board board)
{
    return create_from_position(Board::into_fen(board)).value();
}

std::string Match::get_moves() const
{
    if (m_state.size() == 1) {
        return "";
    }

    std::string result { m_state.at(1).first.lan() };

    for (int i = 2; i < (int)m_state.size(); ++i) {
        result += " " + m_state.at(i).first.lan();
    }

    return result;
}
