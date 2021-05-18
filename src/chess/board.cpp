#include <regex>
#include <vector>
#include "board.hpp"

// ^((?:[pbnrqkPBNRQK1-8]+\/){7}[pbnrqkPBNRQK1-8]+) ([wb]{1})( (?! )K?Q?k?q? | - )((?:[a-h]{1}[36]{1})|-) (\d+) (\d+)$

// Yoinked from: https://stackoverflow.com/a/46931770
std::vector<std::string> split(std::string string, std::string delimiter)
{
    size_t pos_start = 0;
    size_t pos_end = 0;
    size_t delim_len = delimiter.length();

    std::string token;
    std::vector<std::string> result;

    while ((pos_end = string.find(delimiter, pos_start)) != std::string::npos) {
        token = string.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        result.push_back(token);
    }

    result.push_back(string.substr(pos_start));

    return result;
}

Board::Board()
{
    pieces.fill(Piece());
}

Piece Board::get(std::string coordinates)
{
    return pieces.at(Board::parse_coordinates(coordinates));
}

Piece Board::get(unsigned int x, unsigned int y)
{
    return pieces.at(y * BOARD_WIDTH + x);
}

void Board::move_uci(std::string notation)
{
    // TODO: handle promotions (handle a fifth character).

    auto start_index = Board::parse_coordinates(notation.substr(0, 2));
    auto end_index = Board::parse_coordinates(notation.substr(2, 4));

    if (start_index == -1 || end_index == -1) {
        return;
    }

    auto previous = pieces.at(start_index);
    if (previous.type == PieceType::None) {
        return;
    }

    pieces.at(start_index) = Piece();
    pieces.at(end_index) = previous;
}

Board Board::load_from_fen(std::string fen)
{
    if (!std::regex_match(fen, std::regex("^((?:[pbnrqkPBNRQK1-8]+\\/){7}[pbnrqkPBNRQK1-8]+) ([wb]{1})( (?! )K?Q?k?q? | - )((?:[a-h]{1}[36]{1})|-) (\\d+) (\\d+)$"))) {
        return Board::load_from_fen(STARTING_FEN);
    }

    auto board = Board();
    auto sections = split(fen, " ");
    // TODO: We still need to consider the other 5 sections!
    auto rows = split(sections[0], "/");
    auto index = 0;

    for (auto row : rows) {
        for (auto i = 0; i < (int)row.size(); ++i) {
            auto character = row.substr(i, 1);

            if (std::regex_match(character, std::regex("\\d"))) {
                index += std::stoi(character);
            }
            else {
                board.pieces[index] = Piece(character.c_str()[0]);
                index++;
            }
        }
    }

    return board;
}

int Board::parse_coordinates(std::string coordinates)
{
    if (!std::regex_match(coordinates, std::regex("[a-h]{1}[1-8]{1}"))) {
        return -1;
    }

    auto temp = coordinates.substr(0, 1).c_str()[0];
    temp = tolower(temp);

    auto column = BOARD_WIDTH - ((int)'h' - (int)temp) - 1;
    auto row = BOARD_HEIGHT - std::stoi(coordinates.substr(1, 2));

    return row * BOARD_WIDTH + column;
}
