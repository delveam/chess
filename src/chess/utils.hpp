#ifndef UTILS_HPP
#define UTILS_HPP
#include <string>

namespace constants {
constexpr int board_width = 8;
constexpr int board_height = 8;
constexpr int board_size = board_width * board_height;
const std::string starting_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
}

enum class PieceType {
    None = 0,
    Pawn = 4,
    Bishop = 8,
    Knight = 16,
    Rook = 32,
    Queen = 64,
    King = 128,
};

enum class Team {
    None = 0,
    White = 1,
    Black = 2,
};

enum class CastlingRights {
    None = 0,
    WhiteKingSide = 1,
    WhiteQueenSide = 2,
    BlackKingSide = 4,
    BlackQueenSide = 8,
};
#endif
