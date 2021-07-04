#ifndef GM_HPP
#define GM_HPP
#include "board.hpp"
#include "move.hpp"

typedef std::set<std::string> MoveSet;
typedef std::map<unsigned int, MoveSet> Moves;
typedef std::array<bool, constants::board_size> DangerZone;

namespace gm {
std::optional<Board> apply_move(const Board& board, Move team);
DangerZone generate_danger_zone(const Board& board, Team team);
std::optional<Moves> generate_moves(const Board& board, Team team);
}
#endif
