#include "match_state.hpp"

std::optional<MatchState> MatchState::submit_move(Move move) const
{
    auto start = move.start();
    auto start_index = start.y() * constants::board_width + start.x();

    // Note that this if statement acts as a guard.
    if (m_moves.count(start_index) == 0) {
        return std::nullopt;
    }

    // Because of the guard above, this if statement (which is also a guard) shouldn't throw an exception.
    if (m_moves.at(start_index).count(move.lan()) == 0) {
        return std::nullopt;
    }

    auto board = gm::apply_move(m_board, move).value();
    auto moves = gm::generate_moves(board, board.current_team());

    return MatchState(board, moves.value());
}

std::optional<MatchState> MatchState::create(std::string fen)
{
    auto board = Board::load_from_fen(fen);

    if (!board.has_value()) {
        return std::nullopt;
    }

    auto moves = gm::generate_moves(board.value(), board.value().current_team());

    return MatchState(board.value(), moves.value());
}
