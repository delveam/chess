#include <cmath>
#include <vector>
#include "gm.hpp"

const MoveSet castle_set = MoveSet{ "e1g1", "e1c1", "e8g8", "e8c8" };

bool target_is(const Board& board, unsigned int x, unsigned int y, Team team)
{
    auto target = board.get(x, y);

    return target.has_value() && target->team() == team;
}

// TODO(thismarvin): This works, but there has to be a better way!
bool target_is_not(const Board& board, unsigned int x, unsigned int y, Team team)
{
    auto target = board.get(x, y);

    return target.has_value() && target->team() != team;
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

void walk(const Board& board, MoveSet& result, Coordinates coords, int dx, int dy, Team current)
{
    auto x = coords.x();
    auto y = coords.y();
    auto other_team = current == Team::White ? Team::Black : Team::White;
    auto size = constants::board_width > constants::board_height ? constants::board_width : constants::board_height;

    for (int i = 1; i < size; ++i) {
        auto target_coords = Coordinates::create(x + i * dx, y + i * dy);
        if (!target_coords.has_value()) {
            break;
        }

        auto index = target_coords->y() * constants::board_width + target_coords->x();
        auto target = board.pieces().at(index);

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

MoveSet generate_pawn_moves(const Board& board, Coordinates coords)
{
    MoveSet result;

    auto x = coords.x();
    auto y = coords.y();
    auto index = y * constants::board_width + x;
    auto current = board.pieces().at(index);

    if (current.team() == Team::None) {
        return result;
    }

    switch (current.team()) {
    case Team::White: {
        if (target_is(board, x, y - 1, Team::None)) {
            auto target_coords = Coordinates::create(x, y - 1).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
        if (y == 6 && target_is(board, x, y - 1, Team::None) && target_is(board, x, y - 2, Team::None)) {
            auto target_coords = Coordinates::create(x, y - 2).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
        if (target_is(board, x - 1, y - 1, Team::Black)) {
            auto target_coords = Coordinates::create(x - 1, y - 1).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
        if (target_is(board, x + 1, y - 1, Team::Black)) {
            auto target_coords = Coordinates::create(x + 1, y - 1).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
        if (y == 3 && board.en_passant_target().has_value()) {
            result.insert(coords.to_string() + board.en_passant_target().value());
        }
        break;
    }

    case Team::Black: {
        if (target_is(board, x, y + 1, Team::None)) {
            auto target_coords = Coordinates::create(x, y + 1).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
        if (y == 1 && target_is(board, x, y + 1, Team::None) && target_is(board, x, y + 2, Team::None)) {
            auto target_coords = Coordinates::create(x, y + 2).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
        if (target_is(board, x - 1, y + 1, Team::White)) {
            auto target_coords = Coordinates::create(x - 1, y + 1).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
        if (target_is(board, x + 1, y + 1, Team::White)) {
            auto target_coords = Coordinates::create(x + 1, y + 1).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
        if (y == 4 && board.en_passant_target().has_value()) {
            result.insert(coords.to_string() + board.en_passant_target().value());
        }
        break;
    }

    default:
        break;
    }

    return result;
}

MoveSet generate_knight_moves(const Board& board, Coordinates coords)
{
    MoveSet result;

    auto x = coords.x();
    auto y = coords.y();
    auto index = y * constants::board_width + x;
    auto current = board.pieces().at(index);
    auto team = current.team();

    if (team == Team::None) {
        return result;
    }

    if (target_is_not(board, x + 1, y - 2, team)) {
        auto target_coords = Coordinates::create(x + 1, y - 2).value();
        result.insert(coords.to_string() + target_coords.to_string());
    }
    if (target_is_not(board, x + 2, y - 1, team)) {
        auto target_coords = Coordinates::create(x + 2, y - 1).value();
        result.insert(coords.to_string() + target_coords.to_string());
    }
    if (target_is_not(board, x + 2, y + 1, team)) {
        auto target_coords = Coordinates::create(x + 2, y + 1).value();
        result.insert(coords.to_string() + target_coords.to_string());
    }
    if (target_is_not(board, x + 1, y + 2, team)) {
        auto target_coords = Coordinates::create(x + 1, y + 2).value();
        result.insert(coords.to_string() + target_coords.to_string());
    }
    if (target_is_not(board, x - 1, y + 2, team)) {
        auto target_coords = Coordinates::create(x - 1, y + 2).value();
        result.insert(coords.to_string() + target_coords.to_string());
    }
    if (target_is_not(board, x - 2, y + 1, team)) {
        auto target_coords = Coordinates::create(x - 2, y + 1).value();
        result.insert(coords.to_string() + target_coords.to_string());
    }
    if (target_is_not(board, x - 2, y - 1, team)) {
        auto target_coords = Coordinates::create(x - 2, y - 1).value();
        result.insert(coords.to_string() + target_coords.to_string());
    }
    if (target_is_not(board, x - 1, y - 2, team)) {
        auto target_coords = Coordinates::create(x - 1, y - 2).value();
        result.insert(coords.to_string() + target_coords.to_string());
    }

    return result;
}

MoveSet generate_bishop_moves(const Board& board, Coordinates coords)
{
    MoveSet result;

    auto x = coords.x();
    auto y = coords.y();
    auto index = y * constants::board_width + x;
    auto current = board.pieces().at(index);
    auto team = current.team();

    if (team == Team::None) {
        return result;
    }

    walk(board, result, coords, 1, -1, team);
    walk(board, result, coords, 1, 1, team);
    walk(board, result, coords, -1, 1, team);
    walk(board, result, coords, -1, -1, team);

    return result;
}

MoveSet generate_rook_moves(const Board& board, Coordinates coords)
{
    MoveSet result;

    auto x = coords.x();
    auto y = coords.y();
    auto index = y * constants::board_width + x;
    auto current = board.pieces().at(index);
    auto team = current.team();

    if (team == Team::None) {
        return result;
    }

    walk(board, result, coords, 0, -1, team);
    walk(board, result, coords, 1, 0, team);
    walk(board, result, coords, 0, 1, team);
    walk(board, result, coords, -1, 0, team);

    return result;
}

MoveSet generate_queen_moves(const Board& board, Coordinates coords)
{
    MoveSet result;

    auto x = coords.x();
    auto y = coords.y();
    auto index = y * constants::board_width + x;
    auto current = board.pieces().at(index);
    auto team = current.team();

    if (team == Team::None) {
        return result;
    }

    walk(board, result, coords, 1, -1, team);
    walk(board, result, coords, 1, 1, team);
    walk(board, result, coords, -1, 1, team);
    walk(board, result, coords, -1, -1, team);
    walk(board, result, coords, 0, -1, team);
    walk(board, result, coords, 1, 0, team);
    walk(board, result, coords, 0, 1, team);
    walk(board, result, coords, -1, 0, team);

    return result;
}

MoveSet generate_king_moves(const Board& board, Coordinates coords)
{
    MoveSet result;

    auto x = coords.x();
    auto y = coords.y();
    auto index = y * constants::board_width + x;
    auto current = board.pieces().at(index);
    auto team = current.team();

    if (team == Team::None) {
        return result;
    }

    if (target_is_not(board, x, y - 1, team)) {
        auto target_coords = Coordinates::create(x, y - 1).value();
        result.insert(coords.to_string() + target_coords.to_string());
    }
    if (target_is_not(board, x + 1, y - 1, team)) {
        auto target_coords = Coordinates::create(x + 1, y - 1).value();
        result.insert(coords.to_string() + target_coords.to_string());
    }
    if (target_is_not(board, x + 1, y, team)) {
        auto target_coords = Coordinates::create(x + 1, y).value();
        result.insert(coords.to_string() + target_coords.to_string());
    }
    if (target_is_not(board, x + 1, y + 1, team)) {
        auto target_coords = Coordinates::create(x + 1, y + 1).value();
        result.insert(coords.to_string() + target_coords.to_string());
    }
    if (target_is_not(board, x, y + 1, team)) {
        auto target_coords = Coordinates::create(x, y + 1).value();
        result.insert(coords.to_string() + target_coords.to_string());
    }
    if (target_is_not(board, x - 1, y + 1, team)) {
        auto target_coords = Coordinates::create(x - 1, y + 1).value();
        result.insert(coords.to_string() + target_coords.to_string());
    }
    if (target_is_not(board, x - 1, y, team)) {
        auto target_coords = Coordinates::create(x - 1, y).value();
        result.insert(coords.to_string() + target_coords.to_string());
    }
    if (target_is_not(board, x - 1, y - 1, team)) {
        auto target_coords = Coordinates::create(x - 1, y - 1).value();
        result.insert(coords.to_string() + target_coords.to_string());
    }

    // Handle castling.
    auto king_side = team == Team::White ? CastlingRights::WhiteKingSide : CastlingRights::BlackKingSide;
    auto queen_side = team == Team::White ? CastlingRights::WhiteQueenSide : CastlingRights::BlackQueenSide;

    if (contains_castling_right(board.castling_rights(), king_side)) {
        if (target_is(board, x + 1, y, Team::None) && target_is(board, x + 2, y, Team::None)) {
            auto target_coords = Coordinates::create(x + 2, y).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
    }

    if (contains_castling_right(board.castling_rights(), queen_side)) {
        if (target_is(board, x - 1, y, Team::None) && target_is(board, x - 2, y, Team::None) && target_is(board, x - 3, y, Team::None)) {
            auto target_coords = Coordinates::create(x - 2, y).value();
            result.insert(coords.to_string() + target_coords.to_string());
        }
    }

    return result;
}

Moves generate_move_canidates(const Board& board, Team team)
{
    Moves result;

    for (int y = 0; y < constants::board_height; ++y) {
        for (int x = 0; x < constants::board_width; ++x) {
            auto index = y * constants::board_width + x;
            auto target = board.pieces().at(index);
            auto coords = Coordinates::create(x, y).value();

            if (target.team() != team) {
                continue;
            }

            std::set<std::string> moves;

            switch (target.type()) {
            case PieceType::Pawn:
                moves = generate_pawn_moves(board, coords);
                break;
            case PieceType::Knight:
                moves = generate_knight_moves(board, coords);
                break;
            case PieceType::Bishop:
                moves = generate_bishop_moves(board, coords);
                break;
            case PieceType::Rook:
                moves = generate_rook_moves(board, coords);
                break;
            case PieceType::Queen:
                moves = generate_queen_moves(board, coords);
                break;
            case PieceType::King:
                moves = generate_king_moves(board, coords);
                break;
            default:
                break;
            }

            result.try_emplace(index, moves);
        }
    }

    return result;
}

std::optional<unsigned int> find_king(const Board& board, Team team)
{
    for (int i = 0; i < (int)board.pieces().size(); ++i) {
        auto target = board.pieces()[i];
        if (target.type() == PieceType::King && target.team() == team) {
            return i;
        }
    }

    return std::nullopt;
}

bool being_attacked(const Board& board, unsigned int target_index)
{
    auto other_team = board.pieces().at(target_index).team() == Team::White ? Team::Black : Team::White;
    auto opponents_moves = generate_move_canidates(board, other_team);

    for (const auto& pair : opponents_moves) {
        auto attacker = board.pieces().at(pair.first);
        auto x = pair.first % constants::board_width;
        auto y = (int)trunc((float)pair.first / constants::board_width);

        // We do not care about Pawn advances, just Pawn captures. The current MoveSet doesn't help us here,
        // so we roll our own logic!
        if (attacker.type() == PieceType::Pawn) {
            std::optional<Coordinates> left = std::nullopt;
            std::optional<Coordinates> right = std::nullopt;

            switch (attacker.team()) {
            case Team::White: {
                left = Coordinates::create(x - 1, y - 1);
                right = Coordinates::create(x + 1, y - 1);
                break;
            }
            case Team::Black: {
                left = Coordinates::create(x - 1, y + 1);
                right = Coordinates::create(x + 1, y + 1);
                break;
            }
            default:
                break;
            }

            if (left.has_value()) {
                auto index = left->y() * constants::board_width + left->x();
                if (index == target_index) {
                    return true;
                }
            }
            if (right.has_value()) {
                auto index = right->y() * constants::board_width + right->x();
                if (index == target_index) {
                    return true;
                }
            }

            // HEY! Notice me! I exist to prevent unnecessary nesting.
            continue;
        }

        // Once Pawns are out of the equation, every potential move is considered dangerous!
        for (const auto& entry : pair.second) {
            auto coords = Coordinates::from_string(entry.substr(2, 4)).value();
            auto index = coords.y() * constants::board_width + coords.x();
            if (index == target_index) {
                return true;
            }
        }
    }

    return false;
}

bool can_move(const Moves& moves)
{
    for (auto const& pair : moves) {
        if (pair.second.size() > 0) {
            return true;
        }
    }

    return false;
}

// Create an array that represents every possible square the opposing team can capture.
DangerZone generate_danger_zone(const Board& board, Team team)
{
    DangerZone result;
    result.fill(false);

    auto other_team = team == Team::White ? Team::Black : Team::White;
    auto opponents_moves = generate_move_canidates(board, other_team);

    // Generate the DangerZone of our opponent's moves.
    for (const auto& pair : opponents_moves) {
        auto attacker = board.pieces().at(pair.first);

        auto x = pair.first % constants::board_width;
        auto y = (int)trunc((float)pair.first / constants::board_width);

        // We do not care about Pawn advances, just Pawn captures. The current MoveSet doesn't help us here,
        // so we roll our own logic!
        if (attacker.type() == PieceType::Pawn) {
            std::optional<Coordinates> left = std::nullopt;
            std::optional<Coordinates> right = std::nullopt;

            switch (attacker.team()) {
            case Team::White: {
                left = Coordinates::create(x - 1, y - 1);
                right = Coordinates::create(x + 1, y - 1);
                break;
            }
            case Team::Black: {
                left = Coordinates::create(x - 1, y + 1);
                right = Coordinates::create(x + 1, y + 1);
                break;
            }
            default:
                break;
            }

            if (left.has_value()) {
                auto index = left->y() * constants::board_width + left->x();
                result[index] = true;
            }
            if (right.has_value()) {
                auto index = right->y() * constants::board_width + right->x();
                result[index] = true;
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

std::optional<Board> gm::apply_move(const Board& board, Move move)
{
    auto start_index = move.start().y() * constants::board_width + move.start().x();
    auto end_index = move.end().y() * constants::board_width + move.end().x();

    auto previous = board.pieces().at(start_index);

    auto dx = (int)move.end().x() - (int)move.start().x();
    auto dy = (int)move.end().y() - (int)move.start().y();
    auto was_capture = board.pieces().at(end_index).type() != PieceType::None;

    // Setup next Board.
    Pieces pieces;
    auto current_team = board.current_team() == Team::White ? Team::Black : Team::White;
    auto castling_rights = board.castling_rights();
    std::optional<std::string> en_passant_target = std::nullopt;
    auto half_moves = board.half_moves() + 1;
    auto full_moves = board.full_moves();

    // Copy the current board.
    for (int i = 0; i < (int)board.pieces().size(); ++i) {
        pieces.at(i) = board.pieces()[i];
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
        auto temp = Coordinates::create(move.start().x(), move.start().y() + direction).value();
        en_passant_target = temp.to_string();
    }

    // Deal with an en passant (Holy hell).
    if (previous.type() == PieceType::Pawn && board.en_passant_target().has_value() && move.end().to_string() == board.en_passant_target().value()) {
        auto temp = Coordinates::from_string(board.en_passant_target().value()).value();
        auto direction = dy > 0 ? -1 : 1;
        auto index = (temp.y() + direction) * constants::board_width + temp.x();
        pieces.at(index) = Piece();
    }

    if (was_capture || previous.type() == PieceType::Pawn) {
        half_moves = 0;
    }

    if (board.current_team() == Team::Black) {
        ++full_moves;
    }

    // Move the piece.
    pieces.at(start_index) = Piece();
    pieces.at(end_index) = previous;

    return Board(pieces, current_team, castling_rights, en_passant_target, half_moves, full_moves);
}

std::optional<gm::Analysis> gm::analyze(const Board& board, Team team)
{
    auto king_index = find_king(board, team);

    if (!king_index.has_value()) {
        return std::nullopt;
    }

    auto moves = generate_move_canidates(board, team);
    auto danger_zone = generate_danger_zone(board, team);

    std::vector<std::string> move_deletion_queue;

    for (auto& pair : moves) {
        move_deletion_queue.clear();

        // We need to make sure that each move does not cause the King to be in check; however, if we
        // verify every move then the application slows down.
        //
        // We know that the King's moves should always be verified, but what about the other pieces?
        // At the moment we only verify the other pieces if the King is currently in check or if the piece
        // is being attacked. This seems to be working, but further testing is required.
        if (pair.first != king_index.value() && !danger_zone[king_index.value()] && !danger_zone[pair.first]) {
            continue;
        }

        // TODO(thismarvin): Verifying that a move is legal is expensive. We need to figure out why...
        for (auto const& move : pair.second) {
            // If the King is in check then make sure it cannot castle.
            if (pair.first == king_index.value() && danger_zone[pair.first] && castle_set.count(move) > 0) {
                move_deletion_queue.push_back(move);
                continue;
            }

            auto temp_board = gm::apply_move(board, Move::create(move).value()).value();

            // Now that we have applied the move to the board, we need to make sure the King is not in check.
            // If we happen to have moved the King then we need to recalculate their index.
            if (pair.first == king_index.value()) {
                auto new_king_index = find_king(temp_board, team).value();
                if (being_attacked(temp_board, new_king_index)) {
                    move_deletion_queue.push_back(move);
                }
                continue;
            }

            if (being_attacked(temp_board, king_index.value())) {
                move_deletion_queue.push_back(move);
            }
        }

        for (auto const& move : move_deletion_queue) {
            pair.second.erase(move);
        }
    }

    auto event = gm::Event::None;
    if (danger_zone[king_index.value()]) {
        event = can_move(moves) ? gm::Event::Check : gm::Event::Checkmate;
    }

    return Analysis(moves, danger_zone, event);
}
