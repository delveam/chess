#include <cmath>
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
std::optional<Piece> get_piece(const Pieces& pieces, unsigned int x, unsigned int y)
{
    if (x < 0 || x >= constants::board_width || y < 0 || y >= constants::board_height) {
        return std::nullopt;
    }

    return pieces.at(y * constants::board_width + x);
}

bool target_is(const Pieces& pieces, unsigned int x, unsigned int y, Team team)
{
    auto target = get_piece(pieces, x, y);

    return target.has_value() && target->team() == team;
}

// TODO(thismarvin): This works, but there has to be a better way!
bool target_is_not(const Pieces& pieces, unsigned int x, unsigned int y, Team team)
{
    auto target = get_piece(pieces, x, y);

    return target.has_value() && target->team() != team;
}

void walk(const Pieces& pieces, MoveSet& result, Coordinates coords, int dx, int dy, Team current_team)
{
    auto x = coords.x();
    auto y = coords.y();
    auto other_team = current_team == Team::White ? Team::Black : Team::White;
    auto size = constants::board_width > constants::board_height ? constants::board_width : constants::board_height;

    for (int i = 1; i < size; ++i) {
        auto target_coords = Coordinates::create(x + i * dx, y + i * dy);
        if (!target_coords.has_value()) {
            break;
        }

        auto index = target_coords->y() * constants::board_width + target_coords->x();
        auto target = pieces.at(index);

        if (target.team() == Team::None) {
            result.insert(coords.to_string() + target_coords.value().to_string());
            continue;
        }

        if (target.team() == other_team) {
            result.insert(coords.to_string() + target_coords.value().to_string());
        }

        break;
    }
}

bool contains_castling_right(CastlingRights a, CastlingRights b)
{
    return (static_cast<int>(a) & static_cast<int>(b)) != static_cast<int>(CastlingRights::None);
}

CastlingRights remove_castling_right(CastlingRights a, CastlingRights b)
{
    if (contains_castling_right(a, b)) {
        return static_cast<CastlingRights>(static_cast<int>(a) ^ static_cast<int>(b));
    }
    return a;
}

// TODO(thismarvin): This works, but the parameters are completly wrong! However, I cannot fix them until Board is refactored :(
DangerZone generate_danger_zone(const Pieces& pieces, const Moves& moves, Team opponent)
{
    DangerZone result;
    result.fill(false);

    for (const auto& pair : moves) {
        auto index = pair.first;
        auto start = pieces.at(index);

        if (start.team() != opponent) {
            continue;
        }

        auto x = index % constants::board_width;
        auto y = (int)trunc((float)index / constants::board_width);

        // We do not care about Pawn advances, just Pawn captures. The current MoveSet doesn't help us here, so we roll our own logic!
        if (start.type() == PieceType::Pawn) {
            switch (opponent) {
            case Team::White: {
                auto left = Coordinates::create(x - 1, y - 1);
                auto right = Coordinates::create(x + 1, y - 1);
                if (left.has_value()) {
                    auto index = left->y() * constants::board_width + left->x();
                    result[index] = true;
                }
                if (right.has_value()) {
                    auto index = right->y() * constants::board_width + right->x();
                    result[index] = true;
                }
                break;
            }
            case Team::Black: {
                auto left = Coordinates::create(x - 1, y + 1);
                auto right = Coordinates::create(x + 1, y + 1);
                if (left.has_value()) {
                    auto index = left->y() * constants::board_width + left->x();
                    result[index] = true;
                }
                if (right.has_value()) {
                    auto index = right->y() * constants::board_width + right->x();
                    result[index] = true;
                }
                break;
            }
            default:
                break;
            }

            // HEY! Notice me! I exist to prevent unnecessary nesting.
            continue;
        }

        // Once Pawns are out of the equation, every potential move is considered dangerous!
        for (const auto& entry : pair.second) {
            auto coords = Coordinates::from_string(entry.substr(2, 4)).value();
            auto index = coords.y() * constants::board_width + coords.x();
            result[index] = true;
        }
    }

    return result;
}

Board::Board()
{
    m_moves = generate_move_map();
    m_danger_zone = generate_danger_zone(m_pieces, m_moves, m_current_team);
}

Board::Board(Pieces pieces, Team current_team, CastlingRights castling_rights, std::optional<std::string> en_passant_target, unsigned int half_moves, unsigned int full_moves) :
    m_pieces(pieces),
    m_current_team(current_team),
    m_castling_rights(castling_rights),
    m_en_passant_target(en_passant_target),
    m_half_moves(half_moves),
    m_full_moves(full_moves)
{
    m_moves = generate_move_map();
    m_danger_zone = generate_danger_zone(m_pieces, m_moves, m_current_team);
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
        if (y == 3 && m_en_passant_target.has_value()) {
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
        if (y == 4 && m_en_passant_target.has_value()) {
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

    walk(m_pieces, result, coords, 1, -1, team);
    walk(m_pieces, result, coords, 1, 1, team);
    walk(m_pieces, result, coords, -1, 1, team);
    walk(m_pieces, result, coords, -1, -1, team);

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

    walk(m_pieces, result, coords, 0, -1, team);
    walk(m_pieces, result, coords, 1, 0, team);
    walk(m_pieces, result, coords, 0, 1, team);
    walk(m_pieces, result, coords, -1, 0, team);

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

    walk(m_pieces, result, coords, 1, -1, team);
    walk(m_pieces, result, coords, 1, 1, team);
    walk(m_pieces, result, coords, -1, 1, team);
    walk(m_pieces, result, coords, -1, -1, team);
    walk(m_pieces, result, coords, 0, -1, team);
    walk(m_pieces, result, coords, 1, 0, team);
    walk(m_pieces, result, coords, 0, 1, team);
    walk(m_pieces, result, coords, -1, 0, team);

    return result;
}

MoveSet Board::generate_king_moves(Coordinates coords) const
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

    if (target_is_not(m_pieces, x, y - 1, team)) {
        auto target_coords = Coordinates::create(x, y - 1).value();
        result.insert(coords.to_string() + target_coords.to_string());
    }
    if (target_is_not(m_pieces, x + 1, y - 1, team)) {
        auto target_coords = Coordinates::create(x + 1, y - 1).value();
        result.insert(coords.to_string() + target_coords.to_string());
    }
    if (target_is_not(m_pieces, x + 1, y, team)) {
        auto target_coords = Coordinates::create(x + 1, y).value();
        result.insert(coords.to_string() + target_coords.to_string());
    }
    if (target_is_not(m_pieces, x + 1, y + 1, team)) {
        auto target_coords = Coordinates::create(x + 1, y + 1).value();
        result.insert(coords.to_string() + target_coords.to_string());
    }
    if (target_is_not(m_pieces, x, y + 1, team)) {
        auto target_coords = Coordinates::create(x, y + 1).value();
        result.insert(coords.to_string() + target_coords.to_string());
    }
    if (target_is_not(m_pieces, x - 1, y + 1, team)) {
        auto target_coords = Coordinates::create(x - 1, y + 1).value();
        result.insert(coords.to_string() + target_coords.to_string());
    }
    if (target_is_not(m_pieces, x - 1, y, team)) {
        auto target_coords = Coordinates::create(x - 1, y).value();
        result.insert(coords.to_string() + target_coords.to_string());
    }
    if (target_is_not(m_pieces, x - 1, y - 1, team)) {
        auto target_coords = Coordinates::create(x - 1, y - 1).value();
        result.insert(coords.to_string() + target_coords.to_string());
    }

    // Handle castling.
    // TODO(thismarvin): We are not checking if the king is moving through a check!
    auto king_side = team == Team::White ? CastlingRights::WhiteKingSide : CastlingRights::BlackKingSide;
    auto queen_side = team == Team::White ? CastlingRights::WhiteQueenSide : CastlingRights::BlackQueenSide;

    if ((static_cast<int>(m_castling_rights) & static_cast<int>(king_side)) != static_cast<int>(CastlingRights::None)) {
        if (target_is(m_pieces, x + 1, y, Team::None) && target_is(m_pieces, x + 2, y, Team::None)) {
            auto target_coords = Coordinates::create(x + 2, y).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
    }

    if ((static_cast<int>(m_castling_rights) & static_cast<int>(queen_side)) != static_cast<int>(CastlingRights::None)) {
        if (target_is(m_pieces, x - 1, y, Team::None) && target_is(m_pieces, x - 2, y, Team::None) && target_is(m_pieces, x - 3, y, Team::None)) {
            auto target_coords = Coordinates::create(x - 2, y).value();
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

            if (current.team() != m_current_team) {
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
                moves = generate_king_moves(coords);
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
    auto current_team = m_current_team == Team::White ? Team::Black : Team::White;
    auto castling_rights = m_castling_rights;
    std::optional<std::string> en_passant_target = std::nullopt;
    auto half_moves = m_half_moves + 1;
    auto full_moves = m_full_moves;

    // Copy the current board.
    for (int i = 0; i < (int)m_pieces.size(); ++i) {
        pieces.at(i) = m_pieces[i];
    }

    // Keep castling rights up to date.
    if (previous.type() == PieceType::King) {
        // If the king castled then make sure to move the rook!
        if (abs(dx) == 2) {
            auto y = previous.team() == Team::White ? constants::board_height - 1 : 0;
            if (dx < 0) {
                auto x = 0;
                auto index = y * constants::board_width + x;
                auto temp = pieces.at(index);
                pieces.at(index) = Piece();
                pieces.at(index + 3) = temp;
            }
            else {
                auto x = constants::board_width - 1;
                auto index = y * constants::board_width + x;
                auto temp = pieces.at(index);
                pieces.at(index) = Piece();
                pieces.at(index - 2) = temp;
            }
        }

        // If the king moves then remove their ability to castle.
        switch (previous.team()) {
        case Team::White: {
            castling_rights = remove_castling_right(castling_rights, CastlingRights::WhiteKingSide);
            castling_rights = remove_castling_right(castling_rights, CastlingRights::WhiteQueenSide);
            break;
        }
        case Team::Black: {
            castling_rights = remove_castling_right(castling_rights, CastlingRights::BlackKingSide);
            castling_rights = remove_castling_right(castling_rights, CastlingRights::BlackQueenSide);
            break;
        }
        default:
            break;
        }
    }

    // Make sure that moving a rook affects the king's ability to castle.
    if (previous.type() == PieceType::Rook) {
        switch (previous.team()) {
        case Team::White: {
            if (contains_castling_right(castling_rights, CastlingRights::WhiteKingSide)) {
                auto x = constants::board_width - 1;
                auto y = constants::board_height - 1;
                auto index = y * constants::board_width + x;
                if ((int)start_index == index) {
                    castling_rights = remove_castling_right(castling_rights, CastlingRights::WhiteKingSide);
                }
            }
            if (contains_castling_right(castling_rights, CastlingRights::WhiteQueenSide)) {
                auto x = 0;
                auto y = constants::board_height - 1;
                auto index = y * constants::board_width + x;
                if ((int)start_index == index) {
                    castling_rights = remove_castling_right(castling_rights, CastlingRights::WhiteQueenSide);
                }
            }
            break;
        }
        // TODO(thismarvin): This is very similar to White's case. How can we avoid code duplication?
        case Team::Black: {
            if (contains_castling_right(castling_rights, CastlingRights::BlackKingSide)) {
                auto x = constants::board_width - 1;
                auto y = 0;
                auto index = y * constants::board_width + x;
                if ((int)start_index == index) {
                    castling_rights = remove_castling_right(castling_rights, CastlingRights::BlackKingSide);
                }
            }
            if (contains_castling_right(castling_rights, CastlingRights::BlackQueenSide)) {
                auto x = 0;
                auto y = 0;
                auto index = y * constants::board_width + x;
                if ((int)start_index == index) {
                    castling_rights = remove_castling_right(castling_rights, CastlingRights::BlackQueenSide);
                }
            }
            break;
        }
        default:
            break;
        }
    }

    // Handle setting up a potential en passant.
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
        half_moves = 0;
    }

    if (m_current_team == Team::Black) {
        ++full_moves;
    }

    // Move the piece.
    pieces.at(start_index) = Piece();
    pieces.at(end_index) = previous;

    return Board(pieces, current_team, castling_rights, en_passant_target, half_moves, full_moves);
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
