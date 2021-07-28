#ifndef UTILS_HPP
#define UTILS_HPP
#include <string>
#include <vector>

namespace constants {
constexpr int board_width = 8;
constexpr int board_height = 8;
constexpr int board_size = board_width * board_height;
const std::string fen_regex = "((?:[pbnrqkPBNRQK1-8]+\\/){7}[pbnrqkPBNRQK1-8]+)\\s+([wb])\\s+((?!\\s)K?Q?k?q?|-)\\s+((?:[a-h][36])|-)\\s+(\\d+)\\s+(\\d+)";
const std::string starting_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
}

namespace utils {
std::vector<std::string> split(std::string string, std::string delimiter);
std::vector<std::string> split_whitespace(std::string string);
}

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
