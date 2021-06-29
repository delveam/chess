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

// TODO(thismarvin): These utility functions should be somewhere else...
std::optional<Piece> get_piece(Pieces pieces, unsigned int x, unsigned int y)
{
    if (x < 0 || x >= constants::board_width || y < 0 || y >= constants::board_height) {
        return std::nullopt;
    }

    return pieces.at(y * constants::board_width + x);
}

bool target_is(Pieces pieces, unsigned int x, unsigned int y, Team team)
{
    auto target = get_piece(pieces, x, y);

    return target.has_value() && target->team() == team;
}

// TODO(thismarvin): This works, but there has to be a better way!
bool target_is_not(Pieces pieces, unsigned int x, unsigned int y, Team team)
{
    auto target = get_piece(pieces, x, y);

    return target.has_value() && target->team() != team;
}

Board::Board()
{

}

Board::Board(Pieces pieces, Team next_team, CastlingRights castling_rights, std::optional<std::string> en_passant_target, unsigned int half_moves, unsigned int full_moves) :
    m_pieces(pieces),
    m_next_team(next_team),
    m_castling_rights(castling_rights),
    m_en_passant_target(en_passant_target),
    m_half_moves(half_moves),
    m_full_moves(full_moves)
{
    m_moves = generate_move_map();
}

MoveSet Board::generate_pawn_moves(Coordinates coords) const
{
    MoveSet result;

    auto x = coords.x();
    auto y = coords.y();
    auto index = y * constants::board_width + x;
    auto current = m_pieces.at(index);

    if (current.team() == Team::None) {
        return result;
    }

    switch (current.team()) {
    case Team::White: {
        if (target_is(m_pieces, x, y - 1, Team::None)) {
            auto target_coords = Coordinates::create(x, y - 1).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
        if (y == 6 && target_is(m_pieces, x, y - 1, Team::None) && target_is(m_pieces, x, y - 2, Team::None)) {
            auto target_coords = Coordinates::create(x, y - 2).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
        if (target_is(m_pieces, x - 1, y - 1, Team::Black)) {
            auto target_coords = Coordinates::create(x - 1, y - 1).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
        if (target_is(m_pieces, x + 1, y - 1, Team::Black)) {
            auto target_coords = Coordinates::create(x + 1, y - 1).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
        if (m_en_passant_target.has_value()) {
            result.insert(coords.to_string() + m_en_passant_target.value());
        }
        break;
    }

    case Team::Black: {
        if (target_is(m_pieces, x, y + 1, Team::None)) {
            auto target_coords = Coordinates::create(x, y + 1).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
        if (y == 1 && target_is(m_pieces, x, y + 1, Team::None) && target_is(m_pieces, x, y + 2, Team::None)) {
            auto target_coords = Coordinates::create(x, y + 2).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
        if (target_is(m_pieces, x - 1, y + 1, Team::White)) {
            auto target_coords = Coordinates::create(x - 1, y + 1).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
        if (target_is(m_pieces, x + 1, y + 1, Team::White)) {
            auto target_coords = Coordinates::create(x + 1, y + 1).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
        if (m_en_passant_target.has_value()) {
            result.insert(coords.to_string() + m_en_passant_target.value());
        }
        break;
    }

    default:
        break;
    }

    return result;
}

MoveSet Board::generate_knight_moves(Coordinates coords) const
{
    MoveSet result;

    auto x = coords.x();
    auto y = coords.y();
    auto index = y * constants::board_width + x;
    auto current = m_pieces.at(index);
    auto team = current.team();

    if (team == Team::None) {
        return result;
    }

    if (target_is_not(m_pieces, x + 1, y - 2, team)) {
        auto target_coords = Coordinates::create(x + 1, y - 2).value();
        result.insert(coords.to_string() + target_coords.to_string());
    }
    if (target_is_not(m_pieces, x + 2, y - 1, team)) {
        auto target_coords = Coordinates::create(x + 2, y - 1).value();
        result.insert(coords.to_string() + target_coords.to_string());
    }
    if (target_is_not(m_pieces, x + 2, y + 1, team)) {
        auto target_coords = Coordinates::create(x + 2, y + 1).value();
        result.insert(coords.to_string() + target_coords.to_string());
    }
    if (target_is_not(m_pieces, x + 1, y + 2, team)) {
        auto target_coords = Coordinates::create(x + 1, y + 2).value();
        result.insert(coords.to_string() + target_coords.to_string());
    }
    if (target_is_not(m_pieces, x - 1, y + 2, team)) {
        auto target_coords = Coordinates::create(x - 1, y + 2).value();
        result.insert(coords.to_string() + target_coords.to_string());
    }
    if (target_is_not(m_pieces, x - 2, y + 1, team)) {
        auto target_coords = Coordinates::create(x - 2, y + 1).value();
        result.insert(coords.to_string() + target_coords.to_string());
    }
    if (target_is_not(m_pieces, x - 2, y - 1, team)) {
        auto target_coords = Coordinates::create(x - 2, y - 1).value();
        result.insert(coords.to_string() + target_coords.to_string());
    }
    if (target_is_not(m_pieces, x - 1, y - 2, team)) {
        auto target_coords = Coordinates::create(x - 1, y - 2).value();
        result.insert(coords.to_string() + target_coords.to_string());
    }

    return result;
}

MoveSet Board::generate_bishop_moves(Coordinates coords) const
{
    MoveSet result;

    auto x = coords.x();
    auto y = coords.y();
    auto index = y * constants::board_width + x;
    auto current = m_pieces.at(index);
    auto team = current.team();

    if (team == Team::None) {
        return result;
    }

    // TODO(thismarvin): This always checks the maximum amount of spaces. Can we optimize this easily?
    auto size = constants::board_width > constants::board_height ? constants::board_width : constants::board_height;
    for (int i = 0; i < size; ++i) {
        if (target_is_not(m_pieces, x + i, y - i, team)) {
            auto target_coords = Coordinates::create(x + i, y - i).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
        if (target_is_not(m_pieces, x + i, y + i, team)) {
            auto target_coords = Coordinates::create(x + i, y + i).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
        if (target_is_not(m_pieces, x - i, y + i, team)) {
            auto target_coords = Coordinates::create(x - i, y + i).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
        if (target_is_not(m_pieces, x - i, y - i, team)) {
            auto target_coords = Coordinates::create(x - i, y - i).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
    }

    return result;
}

MoveSet Board::generate_rook_moves(Coordinates coords) const
{
    MoveSet result;

    auto x = coords.x();
    auto y = coords.y();
    auto index = y * constants::board_width + x;
    auto current = m_pieces.at(index);
    auto team = current.team();

    if (team == Team::None) {
        return result;
    }

    // TODO(thismarvin): This always checks the maximum amount of spaces. Can we optimize this easily?
    auto size = constants::board_width > constants::board_height ? constants::board_width : constants::board_height;
    for (int i = 0; i < size; ++i) {
        if (target_is_not(m_pieces, x, y - i, team)) {
            auto target_coords = Coordinates::create(x, y - i).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
        if (target_is_not(m_pieces, x + i, y, team)) {
            auto target_coords = Coordinates::create(x + i, y).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
        if (target_is_not(m_pieces, x, y + i, team)) {
            auto target_coords = Coordinates::create(x, y + i).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
        if (target_is_not(m_pieces, x - i, y, team)) {
            auto target_coords = Coordinates::create(x - i, y).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
    }

    return result;
}

MoveSet Board::generate_queen_moves(Coordinates coords) const
{
    MoveSet result;

    auto x = coords.x();
    auto y = coords.y();
    auto index = y * constants::board_width + x;
    auto current = m_pieces.at(index);
    auto team = current.team();

    if (team == Team::None) {
        return result;
    }

    // TODO(thismarvin): This always checks the maximum amount of spaces. Can we optimize this easily?
    // TODO(thismarvin): This is basically rook/bishop's logic combined. Should we do something about code duplication?
    auto size = constants::board_width > constants::board_height ? constants::board_width : constants::board_height;
    for (int i = 0; i < size; ++i) {
        if (target_is_not(m_pieces, x + i, y - i, team)) {
            auto target_coords = Coordinates::create(x + i, y - i).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
        if (target_is_not(m_pieces, x + i, y + i, team)) {
            auto target_coords = Coordinates::create(x + i, y + i).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
        if (target_is_not(m_pieces, x - i, y + i, team)) {
            auto target_coords = Coordinates::create(x - i, y + i).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
        if (target_is_not(m_pieces, x - i, y - i, team)) {
            auto target_coords = Coordinates::create(x - i, y - i).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
        if (target_is_not(m_pieces, x, y - i, team)) {
            auto target_coords = Coordinates::create(x, y - i).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
        if (target_is_not(m_pieces, x + i, y, team)) {
            auto target_coords = Coordinates::create(x + i, y).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
        if (target_is_not(m_pieces, x, y + i, team)) {
            auto target_coords = Coordinates::create(x, y + i).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
        if (target_is_not(m_pieces, x - i, y, team)) {
            auto target_coords = Coordinates::create(x - i, y).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
    }

    return result;
}

Moves Board::generate_move_map() const
{
    Moves result;

    for (int y = 0; y < constants::board_height; ++y) {
        for (int x = 0; x < constants::board_width; ++x) {
            auto index = y * constants::board_width + x;
            auto current = m_pieces.at(index);
            auto coords = Coordinates::create(x, y).value();

            if (current.team() != m_next_team) {
                continue;
            }

            std::set<std::string> moves;

            switch (current.type()) {
            case PieceType::Pawn:
                moves = generate_pawn_moves(coords);
                break;
            case PieceType::Knight:
                moves = generate_knight_moves(coords);
                break;
            case PieceType::Bishop:
                moves = generate_bishop_moves(coords);
                break;
            case PieceType::Rook:
                moves = generate_rook_moves(coords);
                break;
            case PieceType::Queen:
                moves = generate_queen_moves(coords);
                break;
            case PieceType::King:
                break;
            default:
                break;
            }

            result.try_emplace(index, moves);
        }
    }

    return result;
}

std::optional<Piece> Board::get(unsigned int x, unsigned int y) const
{
    if (x < 0 || x >= constants::board_width || y < 0 || y >= constants::board_height) {
        return std::nullopt;
    }

    return m_pieces.at(y * constants::board_width + x);
}

std::optional<Board> Board::move_uci(std::string notation) const
{
    // TODO: handle promotions (handle a fifth character).

    auto start_coords = Coordinates::from_string(notation.substr(0, 2));
    auto end_coords = Coordinates::from_string(notation.substr(2, 4));

    if (!start_coords.has_value() || !end_coords.has_value()) {
        return std::nullopt;
    }

    auto start_index = start_coords->y() * constants::board_width + start_coords->x();
    auto end_index = end_coords->y() * constants::board_width + end_coords->x();

    auto previous = m_pieces.at(start_index);
    if (previous.type() == PieceType::None) {
        return std::nullopt;
    }

    // Note that this if statement acts as a guard.
    if (m_moves.count(start_index) == 0) {
        return std::nullopt;
    }

    // Because of the guard above, this if statement (which is also a guard) shouldn't throw an exception.
    if (m_moves.at(start_index).count(notation) == 0) {
        return std::nullopt;
    }

    auto dx = (int)end_coords->x() - (int)start_coords->x();
    auto dy = (int)end_coords->y() - (int)start_coords->y();
    auto was_capture = m_pieces.at(end_index).type() != PieceType::None;

    // Setup next Board.
    Pieces pieces;
    auto next_team = m_next_team == Team::White ? Team::Black : Team::White;
    auto castling_rights = m_castling_rights;
    auto en_passant_target = std::make_optional<std::string>();
    auto half_moves = m_half_moves;
    auto full_moves = m_full_moves;

    // Copy the current board.
    for (int i = 0; i < (int)m_pieces.size(); ++i) {
        pieces.at(i) = m_pieces[i];
    }

    // TODO(thismarvin): castling_rights (depends on move validation system).

    // Handle setting up potential en passant.
    if (previous.type() == PieceType::Pawn && abs(dy) == 2) {
        auto direction = dy > 0 ? 1 : -1;
        auto temp = Coordinates::create(start_coords->x(), start_coords->y() + direction).value();
        en_passant_target = temp.to_string();
    }

    // Deal with an en passant (Holy hell).
    if (previous.type() == PieceType::Pawn && m_en_passant_target.has_value() && end_coords->to_string() == m_en_passant_target.value()) {
        auto temp = Coordinates::from_string(m_en_passant_target.value()).value();
        auto direction = dy > 0 ? -1 : 1;
        auto index = (temp.y() + direction) * constants::board_width + temp.x();
        pieces.at(index) = Piece();
    }

    if (was_capture || previous.type() == PieceType::Pawn) {
        ++half_moves;
    }

    if (next_team == Team::Black) {
        ++full_moves;
    }

    // Move the piece.
    pieces.at(start_index) = Piece();
    pieces.at(end_index) = previous;

    return Board(pieces, next_team, castling_rights, en_passant_target, half_moves, full_moves);
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

    auto next_team = sections[1] == "w" ? Team::White : Team::Black;

    auto castling_rights_uint = 0;
    for (int i = 0; i < (int)sections[2].length(); ++i) {
        auto current = sections[2].substr(i, i + 1).c_str()[0];
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

    return Board(pieces, next_team, castling_rights, en_passant_target, half_moves, full_moves);
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
    switch (board.m_next_team) {
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
