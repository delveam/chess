#include <regex>
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

std::optional<Piece> Board::get(unsigned int x, unsigned int y) const
{
    if (x < 0 || x >= constants::board_width || y < 0 || y >= constants::board_height) {
        return std::nullopt;
    }

    return m_pieces.at(y * constants::board_width + x);
}

std::optional<Board> Board::load_from_fen(std::string fen)
{
    if (!std::regex_match(fen, std::regex("^((?:[pbnrqkPBNRQK1-8]+\\/){7}[pbnrqkPBNRQK1-8]+) ([wb]{1})( (?! )K?Q?k?q? | - )((?:[a-h]{1}[36]{1})|-) (\\d+) (\\d+)$"))) {
        return std::nullopt;
    }

    auto sections = split(fen, " ");

    auto rows = split(sections[0], "/");
    auto index = 0;

    Pieces pieces;

    for (auto row : rows) {
        for (auto i = 0; i < (int)row.size(); ++i) {
            auto character = row.substr(i, 1);

            if (std::regex_match(character, std::regex("\\d"))) {
                index += std::stoi(character);
            }
            else {
                pieces.at(index) = Piece(character.c_str()[0]);
                index++;
            }
        }
    }

    auto current_team = sections[1] == "w" ? Team::White : Team::Black;

    auto castling_rights_uint = 0;
    for (int i = 0; i < (int)sections[2].length(); ++i) {
        auto current = sections[2].substr(i, 1).c_str()[0];
        switch (current) {
        case 'K':
            castling_rights_uint |= static_cast<unsigned int>(CastlingRights::WhiteKingSide);
            break;
        case 'Q':
            castling_rights_uint |= static_cast<unsigned int>(CastlingRights::WhiteQueenSide);
            break;
        case 'k':
            castling_rights_uint |= static_cast<unsigned int>(CastlingRights::BlackKingSide);
            break;
        case 'q':
            castling_rights_uint |= static_cast<unsigned int>(CastlingRights::BlackQueenSide);
            break;
        }
    }
    auto castling_rights = static_cast<CastlingRights>(castling_rights_uint);

    std::optional<std::string> en_passant_target = std::nullopt;
    if (sections[3] != "-") {
        en_passant_target = sections[3];
    }

    auto half_moves = std::stoi(sections[4]);

    auto full_moves = std::stoi(sections[5]);

    return Board(pieces, current_team, castling_rights, en_passant_target, half_moves, full_moves);
}

std::optional<std::string> Board::into_fen(Board board)
{
    std::string fen = "";

    for (auto y = 0; y < constants::board_height; ++y) {
        for (auto x = 0; x < constants::board_width; ++x) {
            auto piece = board.get(x,y);

            if (piece->type() == PieceType::None) {
                auto total_empty = 1;
                while (true) {
                    auto next_piece = board.get(x + 1, y);
                    if (next_piece.has_value() && next_piece->type() == PieceType::None) {
                        ++total_empty;
                        ++x;
                    }
                    else {
                        fen += std::to_string(total_empty);
                        break;
                    }
                }
            }
            else {
                fen += piece->to_string();
            }

        }
        if (y < constants::board_height - 1) {
            fen+="/";
        }
    }

    fen += " ";
    switch (board.m_current_team) {
    case Team::White:
        fen += "w";
        break;
    case Team::Black:
        fen += "b";
        break;
    case Team::None:
        return std::nullopt;
    }

    fen += " ";
    auto can_castle = false;
    if (static_cast<int>(board.m_castling_rights) & static_cast<int>(CastlingRights::WhiteKingSide)) {
        fen += "K";
        can_castle = true;
    }
    if (static_cast<int>(board.m_castling_rights) & static_cast<int>(CastlingRights::WhiteQueenSide)) {
        fen += "Q";
        can_castle = true;
    }
    if (static_cast<int>(board.m_castling_rights) & static_cast<int>(CastlingRights::BlackKingSide)) {
        fen += "k";
        can_castle = true;
    }
    if (static_cast<int>(board.m_castling_rights) & static_cast<int>(CastlingRights::BlackQueenSide)) {
        fen += "q";
        can_castle = true;
    }

    if (!can_castle) {
        fen += "-";
    }

    fen += " ";
    if (board.m_en_passant_target.has_value()) {
        fen += board.m_en_passant_target.value();
    }
    else {
        fen += "-";
    }

    fen += " " + std::to_string(board.m_half_moves);

    fen += " " + std::to_string(board.m_full_moves);

    return fen;
}
