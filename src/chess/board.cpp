#include <regex>
#include <vector>
#include "board.hpp"
#include "coordinates.hpp"

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
    next_team = Team::None;
    castling_rights = CastlingRights::None;
    en_passant_target = std::nullopt;
    half_moves = 0;
    full_moves = 1;
}

std::optional<Piece> Board::get(unsigned int x, unsigned int y)
{
    if (x < 0 || x >= BOARD_WIDTH || y < 0 || y >= BOARD_HEIGHT) {
        return std::nullopt;
    }

    return pieces.at(y * BOARD_WIDTH + x);
}

// QUESTION(austin0209): Should this return a new Board? Moreover, should board be immutable?
void Board::move_uci(std::string notation)
{
    // TODO: handle promotions (handle a fifth character).

    auto start_coords = Coordinates::from_string(notation.substr(0, 2));
    auto end_coords = Coordinates::from_string(notation.substr(2, 4));

    if (!start_coords.has_value() || !end_coords.has_value()) {
        return;
    }

    auto start_index = start_coords.value().y * BOARD_WIDTH + start_coords.value().x;
    auto end_index = end_coords.value().y * BOARD_WIDTH + end_coords.value().x;

    auto previous = pieces.at(start_index);
    if (previous.type == PieceType::None) {
        return;
    }

    auto was_capture = pieces.at(end_index).type != PieceType::None;

    pieces.at(start_index) = Piece();
    pieces.at(end_index) = previous;

    if (next_team == Team::Black) {
        ++full_moves;
    }

    next_team = next_team == Team::White ? Team::Black : Team::White;

    // TODO(thismarvin): castling_rights (depends on move validation system).
    // TODO(thismarvin): en_passant_target (also depends on move validation system).

    if (was_capture || previous.type == PieceType::Pawn) {
        ++half_moves;
    }
}

Board Board::load_from_fen(std::string fen)
{
    if (!std::regex_match(fen, std::regex("^((?:[pbnrqkPBNRQK1-8]+\\/){7}[pbnrqkPBNRQK1-8]+) ([wb]{1})( (?! )K?Q?k?q? | - )((?:[a-h]{1}[36]{1})|-) (\\d+) (\\d+)$"))) {
        return Board::load_from_fen(STARTING_FEN);
    }

    auto board = Board();
    auto sections = split(fen, " ");

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

    board.next_team = sections[1] == "w" ? Team::White : Team::Black;

    auto castling_rights = 0;
    for (int i = 0; i < (int)sections[2].length(); ++i) {
        auto current = sections[2].substr(i, i + 1).c_str()[0];
        switch (current) {
        case 'K':
            castling_rights |= static_cast<unsigned int>(CastlingRights::WhiteKingSide);
            break;
        case 'Q':
            castling_rights |= static_cast<unsigned int>(CastlingRights::WhiteQueenSide);
            break;
        case 'k':
            castling_rights |= static_cast<unsigned int>(CastlingRights::BlackKingSide);
            break;
        case 'q':
            castling_rights |= static_cast<unsigned int>(CastlingRights::BlackQueenSide);
            break;
        }
    }
    board.castling_rights = static_cast<CastlingRights>(castling_rights);

    if (sections[3] != "-") {
        board.en_passant_target = sections[3];
    }

    board.half_moves = std::stoi(sections[4]);

    board.full_moves = std::stoi(sections[5]);

    return board;
}

std::optional<std::string> Board::into_fen(Board board)
{
    std::string fen = "";

    for (auto y = 0; y < BOARD_HEIGHT; ++y) {
        for (auto x = 0; x < BOARD_WIDTH; ++x) {
            auto piece = board.get(x,y);

            if (piece->type == PieceType::None) {
                auto total_empty = 1;
                while (true) {
                    auto next_piece = board.get(x + 1, y);
                    if (next_piece.has_value() && next_piece->type == PieceType::None) {
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
        if (y < BOARD_HEIGHT - 1) {
            fen+="/";
        }
    }

    fen += " ";
    switch (board.next_team) {
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
    if (static_cast<int>(board.castling_rights) & static_cast<int>(CastlingRights::WhiteKingSide)) {
        fen += "K";
        can_castle = true;
    }
    if (static_cast<int>(board.castling_rights) & static_cast<int>(CastlingRights::WhiteQueenSide)) {
        fen += "Q";
        can_castle = true;
    }
    if (static_cast<int>(board.castling_rights) & static_cast<int>(CastlingRights::BlackKingSide)) {
        fen += "k";
        can_castle = true;
    }
    if (static_cast<int>(board.castling_rights) & static_cast<int>(CastlingRights::BlackQueenSide)) {
        fen += "q";
        can_castle = true;
    }

    if (!can_castle) {
        fen += "-";
    }

    fen += " ";
    if (board.en_passant_target.has_value()) {
        fen += board.en_passant_target.value();
    }
    else {
        fen += "-";
    }

    fen += " " + std::to_string(board.half_moves);

    fen += " " + std::to_string(board.full_moves);

    return fen;
}
