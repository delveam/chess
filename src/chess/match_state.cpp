#include "match_state.hpp"

std::optional<MatchState> MatchState::submit_move(Move move) const
{
    auto start = move.start();
    auto start_index = start.y() * constants::board_width + start.x();

    // Note that this if statement acts as a guard.
    if (m_analysis.moves().count(start_index) == 0) {
        return std::nullopt;
    }

    // Because of the guard above, this if statement (which is also a guard) shouldn't throw an exception.
    if (m_analysis.moves().at(start_index).count(move.lan()) == 0) {
        return std::nullopt;
    }

    auto board = gm::apply_move(m_board, move).value();
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
