#include <regex>
#include "board.hpp"
#include "utils.hpp"

std::optional<Piece> Board::get(unsigned int x, unsigned int y) const
{
    if (x >= constants::board_width || y >= constants::board_height) {
        return std::nullopt;
    }

    return m_pieces.at(y * constants::board_width + x);
}

std::optional<Board> Board::create(std::string fen)
{
    static const auto regex_fen = std::regex("^" + constants::fen_regex + "$");
    static const auto regex_digit = std::regex("\\d");

    if (!std::regex_match(fen, regex_fen)) {
        return std::nullopt;
    }

    auto sections = utils::split_whitespace(fen);

    auto rows = utils::split(sections[0], "/");
    auto index = 0;

    Pieces pieces;

    for (auto row : rows) {
        for (auto i = 0; i < (int)row.size(); ++i) {
            auto character = row.substr(i, 1);

            if (std::regex_match(character, regex_digit)) {
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
        case 'K': {
            castling_rights_uint |= static_cast<unsigned int>(CastlingRights::WhiteKingSide);
            break;
        }
        case 'Q': {
            castling_rights_uint |= static_cast<unsigned int>(CastlingRights::WhiteQueenSide);
            break;
        }
        case 'k': {
            castling_rights_uint |= static_cast<unsigned int>(CastlingRights::BlackKingSide);
            break;
        }
        case 'q': {
            castling_rights_uint |= static_cast<unsigned int>(CastlingRights::BlackQueenSide);
            break;
        }
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

std::string Board::into_fen(Board board)
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
            fen += "/";
        }
    }

    fen += " ";

    switch (board.m_current_team) {
    case Team::White: {
        fen += "w";
        break;
    }
    case Team::Black: {
        fen += "b";
        break;
    }
    default:
        break;
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
