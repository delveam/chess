#ifndef GM_HPP
#define GM_HPP
#include <vector>
#include "analysis.hpp"
#include "board.hpp"
#include "king_safety.hpp"
#include "move.hpp"

namespace gm {
Board apply_move(const Board& board, Move move);
std::optional<gm::Analysis> analyze(const Board& board, Team team);
bool end_condition(KingSafety king_safety);
std::optional<Board> board_from_sequence(std::string fen, std::string moves);
}
#endif
