#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP
#include <string>

namespace constants {
constexpr int board_width = 8;
constexpr int board_height = 8;
constexpr int board_size = board_width * board_height;
const std::string fen_regex = "((?:[pbnrqkPBNRQK1-8]+\\/){7}[pbnrqkPBNRQK1-8]+)\\s+([wb])\\s+((?!\\s)K?Q?k?q?|-)\\s+((?:[a-h][36])|-)\\s+(\\d+)\\s+(\\d+)";
const std::string starting_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
}
#endif
