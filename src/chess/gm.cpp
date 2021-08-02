#include <cmath>
#include <queue>
#include <vector>
#include "gm.hpp"
#include "utils.hpp"

const MoveSet castle_set = MoveSet { "e1g1", "e1c1", "e8g8", "e8c8" };
const std::array<std::string, 4> promotion_array = std::array<std::string, 4> { "b", "n", "r", "q" };

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

std::optional<unsigned int> get_significant_rook_index(CastlingRights castling_rights)
{
    switch (castling_rights) {
    case CastlingRights::WhiteKingSide: {
        auto x = constants::board_width - 1;
        auto y = constants::board_height - 1;

        return y * constants::board_width + x;
    }
    case CastlingRights::WhiteQueenSide: {
        auto x = 0;
        auto y = constants::board_height - 1;

        return y * constants::board_width + x;
    }
    case CastlingRights::BlackKingSide: {
        auto x = constants::board_width - 1;
        auto y = 0;

        return y * constants::board_width + x;
    }
    case CastlingRights::BlackQueenSide: {
        auto x = 0;
        auto y = 0;

        return y * constants::board_width + x;
    }
    default:
        return std::nullopt;
    }
};

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
            result.insert(coords.to_string() + target_coords->to_string());
            continue;
        }

        if (target.team() == other_team) {
            result.insert(coords.to_string() + target_coords->to_string());
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

    auto register_move = [&](unsigned int x, unsigned int y) {
        auto lan = coords.to_string() + Coordinates::create(x, y)->to_string();

        // If a pawn is at the very top or bottom of the board then we must append all possible promotions to the lan.
        if (y == 0 || y == constants::board_height - 1) {
            for (int i = 0; i < (int)promotion_array.size(); ++i) {
                result.insert(lan + promotion_array[i]);
            }
            return;
        }

        result.insert(lan);
    };

    switch (current.team()) {
    case Team::White: {
        // Handle advancing one square.
        if (target_is(board, x, y - 1, Team::None)) {
            register_move(x, y - 1);
        }
        // Handle advancing two squares (if the pawn has not moved before).
        if (y == 6 && target_is(board, x, y - 1, Team::None) && target_is(board, x, y - 2, Team::None)) {
            register_move(x, y - 2);
        }
        // Handle capturing to the top left.
        if (target_is(board, x - 1, y - 1, Team::Black)) {
            register_move(x - 1, y - 1);
        }
        // Handle capturing to the top right.
        if (target_is(board, x + 1, y - 1, Team::Black)) {
            register_move(x + 1, y - 1);
        }
        // Handle en passant.
        if (y == 3 && board.en_passant_target().has_value()) {
            auto temp = Coordinates::from_string(board.en_passant_target().value()).value();
            if ((temp.x() - 1 == x && temp.y() + 1 == y) || (temp.x() + 1 == x && temp.y() + 1 == y)) {
                result.insert(coords.to_string() + board.en_passant_target().value());
            }
        }
        break;
    }
    case Team::Black: {
        // Handle advancing one square.
        if (target_is(board, x, y + 1, Team::None)) {
            register_move(x, y + 1);
        }
        // Handle advancing two squares (if the pawn has not moved before).
        if (y == 1 && target_is(board, x, y + 1, Team::None) && target_is(board, x, y + 2, Team::None)) {
            register_move(x, y + 2);
        }
        // Handle capturing to the bottom left.
        if (target_is(board, x - 1, y + 1, Team::White)) {
            register_move(x - 1, y + 1);
        }
        // Handle capturing to the bottom right.
        if (target_is(board, x + 1, y + 1, Team::White)) {
            register_move(x + 1, y + 1);
        }
        // Handle en passant.
        if (y == 4 && board.en_passant_target().has_value()) {
            auto temp = Coordinates::from_string(board.en_passant_target().value()).value();
            if ((temp.x() - 1 == x && temp.y() - 1 == y) || (temp.x() + 1 == x && temp.y() - 1 == y)) {
                result.insert(coords.to_string() + board.en_passant_target().value());
            }
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

    auto register_move = [&](unsigned int x, unsigned int y) {
        auto lan = coords.to_string() + Coordinates::create(x, y)->to_string();
        result.insert(lan);
    };

    if (target_is_not(board, x + 1, y - 2, team)) {
        register_move(x + 1, y - 2);
    }
    if (target_is_not(board, x + 2, y - 1, team)) {
        register_move(x + 2, y - 1);
    }
    if (target_is_not(board, x + 2, y + 1, team)) {
        register_move(x + 2, y + 1);
    }
    if (target_is_not(board, x + 1, y + 2, team)) {
        register_move(x + 1, y + 2);
    }
    if (target_is_not(board, x - 1, y + 2, team)) {
        register_move(x - 1, y + 2);
    }
    if (target_is_not(board, x - 2, y + 1, team)) {
        register_move(x - 2, y + 1);
    }
    if (target_is_not(board, x - 2, y - 1, team)) {
        register_move(x - 2, y - 1);
    }
    if (target_is_not(board, x - 1, y - 2, team)) {
        register_move(x - 1, y - 2);
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

    auto register_move = [&](unsigned int x, unsigned int y) {
        auto lan = coords.to_string() + Coordinates::create(x, y)->to_string();
        result.insert(lan);
    };

    if (target_is_not(board, x, y - 1, team)) {
        register_move(x, y - 1);
    }
    if (target_is_not(board, x + 1, y - 1, team)) {
        register_move(x + 1, y - 1);
    }
    if (target_is_not(board, x + 1, y, team)) {
        register_move(x + 1, y);
    }
    if (target_is_not(board, x + 1, y + 1, team)) {
        register_move(x + 1, y + 1);
    }
    if (target_is_not(board, x, y + 1, team)) {
        register_move(x, y + 1);
    }
    if (target_is_not(board, x - 1, y + 1, team)) {
        register_move(x - 1, y + 1);
    }
    if (target_is_not(board, x - 1, y, team)) {
        register_move(x - 1, y);
    }
    if (target_is_not(board, x - 1, y - 1, team)) {
        register_move(x - 1, y - 1);
    }

    // Handle castling.
    auto king_side = team == Team::White ? CastlingRights::WhiteKingSide : CastlingRights::BlackKingSide;
    auto queen_side = team == Team::White ? CastlingRights::WhiteQueenSide : CastlingRights::BlackQueenSide;

    if (contains_castling_right(board.castling_rights(), king_side)) {
        if (target_is(board, x + 1, y, Team::None) && target_is(board, x + 2, y, Team::None)) {
            register_move(x + 2, y);
        }
    }

    if (contains_castling_right(board.castling_rights(), queen_side)) {
        if (target_is(board, x - 1, y, Team::None) && target_is(board, x - 2, y, Team::None) && target_is(board, x - 3, y, Team::None)) {
            register_move(x - 2, y);
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

            if (target.team() != team) {
                continue;
            }

            auto coords = Coordinates::create(x, y).value();

            MoveSet moves;

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
            auto coords = Coordinates::from_string(entry.substr(2, 2)).value();
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
            auto coords = Coordinates::from_string(entry.substr(2, 2)).value();
            auto index = coords.y() * constants::board_width + coords.x();
            result[index] = true;
        }
    }

    return result;
}

Board gm::apply_move(const Board& board, Move move)
{
    auto start_index = move.start().y() * constants::board_width + move.start().x();
    auto end_index = move.end().y() * constants::board_width + move.end().x();

    auto previous = board.pieces().at(start_index);

    if (previous.type() == PieceType::None) {
        return board;
    }

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
            if (start_index == get_significant_rook_index(CastlingRights::WhiteKingSide)) {
                castling_rights = remove_castling_right(castling_rights, CastlingRights::WhiteKingSide);
            }
            else if (start_index == get_significant_rook_index(CastlingRights::WhiteQueenSide)) {
                castling_rights = remove_castling_right(castling_rights, CastlingRights::WhiteQueenSide);
            }
        }
        case Team::Black: {
            if (start_index == get_significant_rook_index(CastlingRights::BlackKingSide)) {
                castling_rights = remove_castling_right(castling_rights, CastlingRights::BlackKingSide);
            }
            else if (start_index == get_significant_rook_index(CastlingRights::BlackQueenSide)) {
                castling_rights = remove_castling_right(castling_rights, CastlingRights::BlackQueenSide);
            }
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

    // Handle a promotion.
    if (previous.type() == PieceType::Pawn && move.promotion().has_value()) {
        previous = Piece(move.promotion().value(), previous.team());
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
            if (pair.first == king_index.value() && castle_set.count(move) > 0) {
                // If the King is in check then make sure it cannot castle.
                if (danger_zone[pair.first]) {
                    move_deletion_queue.push_back(move);
                    continue;
                }

                // Make sure the King can not castle through or into a check.
                auto king_side = team == Team::White ? CastlingRights::WhiteKingSide : CastlingRights::BlackKingSide;
                auto queen_side = team == Team::White ? CastlingRights::WhiteQueenSide : CastlingRights::BlackQueenSide;

                auto x = pair.first % constants::board_width;
                auto y = pair.first / constants::board_width;

                auto coords_to_index = [](unsigned int x, unsigned int y) {
                    return y * constants::board_width + x;
                };

                if (contains_castling_right(board.castling_rights(), king_side)) {
                    if (danger_zone[coords_to_index(x + 1, y)] || danger_zone[coords_to_index(x + 2, y)]) {
                        move_deletion_queue.push_back(move);
                    }
                }

                if (contains_castling_right(board.castling_rights(), queen_side)) {
                    if (danger_zone[coords_to_index(x - 1, y)] || danger_zone[coords_to_index(x - 2, y)] || danger_zone[coords_to_index(x - 3, y)]) {
                        move_deletion_queue.push_back(move);
                    }
                }

                continue;
            }

            auto temp_board = gm::apply_move(board, Move::create(move).value());

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

    auto no_moves = !can_move(moves);
    auto king_location = Coordinates::create(king_index.value() % constants::board_width, (unsigned int)trunc((float)king_index.value() / constants::board_width)).value();
    auto king_safety = gm::KingSafety::Safe;

    if (danger_zone[king_index.value()]) {
        king_safety = no_moves ? gm::KingSafety::Checkmate : gm::KingSafety::Check;
    }
    else if (no_moves) {
        king_safety = gm::KingSafety::Stalemate;
    }

    return Analysis(moves, danger_zone, king_location, king_safety);
}

bool gm::end_condition(KingSafety king_safety)
{
    return king_safety == KingSafety::Checkmate || king_safety == KingSafety::Stalemate;
}

std::optional<Board> gm::board_from_sequence(std::string fen, std::string moves)
{
    auto board = Board::create(fen);

    if (!board.has_value()) {
        return std::nullopt;
    }

    auto entries = utils::split_whitespace(moves);

    if (entries.size() == 0) {
        return std::nullopt;
    }

    std::queue<Move> sanitized;

    for (int i = 0; i < (int)entries.size(); ++i) {
        auto move = Move::create(entries.at(i));

        if (!move.has_value()) {
            return std::nullopt;
        }

        sanitized.push(move.value());
    }

    while (sanitized.size() > 0) {
        auto analysis = analyze(board.value(), board->current_team());

        if (!analysis.has_value()) {
            return std::nullopt;
        }

        auto move = sanitized.front();

        if (!analysis->contains_move(move.lan())) {
            return std::nullopt;
        }

        board = apply_move(board.value(), move);

        sanitized.pop();
    }

    return board;
}

