#include "match_state.hpp"

std::optional<MatchState> MatchState::submit_move(Move move) const
{
    if (!m_analysis.contains_move(move.lan())) {
        return std::nullopt;
    }

    auto board = gm::apply_move(m_board, move);
    auto analysis = gm::analyze(board, board.current_team()).value();

    return MatchState(board, analysis);
}

std::optional<MatchState> MatchState::create(std::string fen)
{
    auto board = Board::from_fen(fen);

    if (!board.has_value()) {
        return std::nullopt;
    }

    auto analysis = gm::analyze(board.value(), board->current_team()).value();

    return MatchState(board.value(), analysis);
}
