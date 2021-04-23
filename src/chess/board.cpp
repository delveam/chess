#include <regex>
#include <vector>
#include "board.hpp"

// ^((?:[pbnrqkPBNRQK1-8]+\/){7}[pbnrqkPBNRQK1-8]+) ([wb]{1})( (?! )Q?K?q?k? | - )((?:[a-h]{1}[36]{1})|-) (\d+) (\d+)$

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

Board Board::load_from_fen(std::string fen)
{
    if (!std::regex_match(fen, std::regex("^((?:[pbnrqkPBNRQK1-8]+\\/){7}[pbnrqkPBNRQK1-8]+) ([wb]{1})( (?! )Q?K?q?k? | - )((?:[a-h]{1}[36]{1})|-) (\\d+) (\\d+)$"))) {
        return Board::load_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w QKqk - 0 1");
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
                auto ascii = (int)character.c_str()[0];
                Piece piece = Piece();
                switch (ascii) {
                // Black
                case 112: // p
                    piece = Piece(PieceType::Pawn, Team::Black);
                    break;
                case 110: // n
                    piece = Piece(PieceType::Knight, Team::Black);
                    break;
                case 98: // b
                    piece = Piece(PieceType::Bishop, Team::Black);
                    break;
                case 114: // r
                    piece = Piece(PieceType::Rook, Team::Black);
                    break;
                case 113: // q
                    piece = Piece(PieceType::Queen, Team::Black);
                    break;
                case 107: // k
                    piece = Piece(PieceType::King, Team::Black);
                    break;

                // White
                case 80: // P
                    piece = Piece(PieceType::Pawn, Team::White);
                    break;
                case 78: // N
                    piece = Piece(PieceType::Knight, Team::White);
                    break;
                case 66: // B
                    piece = Piece(PieceType::Bishop, Team::White);
                    break;
                case 82: // R
                    piece = Piece(PieceType::Rook, Team::White);
                    break;
                case 81: // Q
                    piece = Piece(PieceType::Queen, Team::White);
                    break;
                case 75: // K
                    piece = Piece(PieceType::King, Team::White);
                    break;
                }

                board.pieces[index] = piece;
                index++;
            }
        }
    }

    return board;
}
