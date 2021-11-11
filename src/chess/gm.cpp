#include <queue>
#include <vector>
#include "gm.hpp"
#include "utils.hpp"

const MoveSet castle_set = MoveSet { "e1g1", "e1c1", "e8g8", "e8c8" };
const std::array<std::string, 4> promotion_array = std::array<std::string, 4> { "b", "n", "r", "q" };
const std::array<std::string, constants::board_size> coords_to_string_array = {
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"
};

unsigned int coords_to_uint(std::string coords)
{
    int x = (char)coords.substr(0, 1).c_str()[0] - 'a';
    int y = constants::board_height - std::stoi(coords.substr(1, 1));

    return y * constants::board_width + x;
}

bool coords_are_valid(unsigned int x, unsigned int y)
{
    return x < constants::board_width && y < constants::board_height;
}

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

void walk(const Board& board, MoveSet& result, unsigned int x, unsigned int y, int dx, int dy, Team current)
{
    auto coords = coords_to_string_array.at(y * constants::board_width + x);
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
            result.insert(coords + target_coords->to_string());

            continue;
        }

        if (target.team() == other_team) {
            result.insert(coords + target_coords->to_string());
        }

        break;
    }
}

void walk_dangerously(const Board& board, DangerZone& result, unsigned int x, unsigned int y, int dx, int dy, Team current)
{
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
            result[index] = true;

            continue;
        }

        result[index] = true;

        if (target.type() == PieceType::King && target.team() == other_team) {
            continue;
        }

        break;
    }
}

MoveSet generate_pawn_moves(const Board& board, unsigned int x, unsigned int y)
{
    MoveSet result;

    auto index = y * constants::board_width + x;
    auto coords = coords_to_string_array.at(index);
    auto current = board.pieces().at(index);
    auto team = current.team();

    if (team == Team::None) {
        return result;
    }

    auto register_move = [&](unsigned int x, unsigned int y) {
        auto lan = coords + coords_to_string_array.at(y * constants::board_width + x);

        // If a pawn is at the very top or bottom of the board then we must append all possible promotions to the lan.
        if (y == 0 || y == constants::board_height - 1) {
            for (int i = 0; i < (int)promotion_array.size(); ++i) {
                result.insert(lan + promotion_array[i]);
            }

            return;
        }

        result.insert(lan);
    };

    switch (team) {
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
                result.insert(coords + board.en_passant_target().value());
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
                result.insert(coords + board.en_passant_target().value());
            }
        }
        break;
    }
    default:
        break;
    }

    return result;
}

MoveSet generate_knight_moves(const Board& board, unsigned int x, unsigned int y)
{
    MoveSet result;

    auto index = y * constants::board_width + x;
    auto coords = coords_to_string_array.at(index);
    auto current = board.pieces().at(index);
    auto team = current.team();

    if (team == Team::None) {
        return result;
    }

    auto register_move = [&](unsigned int x, unsigned int y) {
        auto lan = coords + coords_to_string_array.at(y * constants::board_width + x);

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

MoveSet generate_bishop_moves(const Board& board, unsigned int x, unsigned int y)
{
    MoveSet result;

    auto index = y * constants::board_width + x;
    auto current = board.pieces().at(index);
    auto team = current.team();

    if (team == Team::None) {
        return result;
    }

    walk(board, result, x, y, 1, -1, team);
    walk(board, result, x, y, 1, 1, team);
    walk(board, result, x, y, -1, 1, team);
    walk(board, result, x, y, -1, -1, team);

    return result;
}

MoveSet generate_rook_moves(const Board& board, unsigned int x, unsigned int y)
{
    MoveSet result;

    auto index = y * constants::board_width + x;
    auto current = board.pieces().at(index);
    auto team = current.team();

    if (team == Team::None) {
        return result;
    }

    walk(board, result, x, y, 0, -1, team);
    walk(board, result, x, y, 1, 0, team);
    walk(board, result, x, y, 0, 1, team);
    walk(board, result, x, y, -1, 0, team);

    return result;
}

MoveSet generate_queen_moves(const Board& board, unsigned int x, unsigned int y)
{
    MoveSet result;

    auto index = y * constants::board_width + x;
    auto current = board.pieces().at(index);
    auto team = current.team();

    if (team == Team::None) {
        return result;
    }

    walk(board, result, x, y, 1, -1, team);
    walk(board, result, x, y, 1, 1, team);
    walk(board, result, x, y, -1, 1, team);
    walk(board, result, x, y, -1, -1, team);
    walk(board, result, x, y, 0, -1, team);
    walk(board, result, x, y, 1, 0, team);
    walk(board, result, x, y, 0, 1, team);
    walk(board, result, x, y, -1, 0, team);

    return result;
}

MoveSet generate_king_moves(const Board& board, unsigned int x, unsigned int y)
{
    MoveSet result;

    auto index = y * constants::board_width + x;
    auto coords = coords_to_string_array.at(index);
    auto current = board.pieces().at(index);
    auto team = current.team();

    if (team == Team::None) {
        return result;
    }

    auto register_move = [&](unsigned int x, unsigned int y) {
        auto lan = coords + coords_to_string_array.at(y * constants::board_width + x);
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

Moves generate_move_candidates(const Board& board, Team team)
{
    Moves result;

    for (int y = 0; y < constants::board_height; ++y) {
        for (int x = 0; x < constants::board_width; ++x) {
            auto index = y * constants::board_width + x;
            auto target = board.pieces().at(index);

            if (target.team() != team) {
                continue;
            }

            MoveSet moves;

            switch (target.type()) {
            case PieceType::Pawn:
                moves = generate_pawn_moves(board, x, y);
                break;
            case PieceType::Knight:
                moves = generate_knight_moves(board, x, y);
                break;
            case PieceType::Bishop:
                moves = generate_bishop_moves(board, x, y);
                break;
            case PieceType::Rook:
                moves = generate_rook_moves(board, x, y);
                break;
            case PieceType::Queen:
                moves = generate_queen_moves(board, x, y);
                break;
            case PieceType::King:
                moves = generate_king_moves(board, x, y);
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

std::optional<std::set<std::string>> find_pins(const Board& board, Team team)
{
    auto king_index = find_king(board, team);

    if (!king_index.has_value()) {
        return std::nullopt;
    }

    std::set<std::string> result;

    auto king_x = king_index.value() % constants::board_width;
    auto king_y = king_index.value() / constants::board_width;

    auto opponent = team == Team::White ? Team::Black : Team::White;

    for (int y = 0; y < constants::board_height; ++y) {
        for (int x = 0; x < constants::board_width; ++x) {
            unsigned int index = y * constants::board_width + x;
            auto target = board.pieces().at(index);
            auto target_x = index % constants::board_width;
            auto target_y = index / constants::board_width;

            if (target.team() == opponent) {
                int dir_x = 0;
                int dir_y = 0;

                switch (target.type()) {
                case PieceType::Bishop: {
                    if (target_x == king_x || target_y == king_y) {
                        continue;
                    }

                    auto difference_x = (int)king_x - (int)target_x;
                    auto difference_y = (int)king_y - (int)target_y;

                    if (abs(difference_x) != abs(difference_y)) {
                        continue;
                    }

                    dir_x = target_x > king_x ? -1 : 1;
                    dir_y = target_y > king_y ? -1 : 1;

                    break;
                }
                case PieceType::Rook: {
                    if (target_x != king_x && target_y != king_y) {
                        continue;
                    }

                    dir_x = target_y != king_y ? 0 : (target_x > king_x ? -1 : 1);
                    dir_y = target_x != king_x ? 0 : (target_y > king_y ? -1 : 1);

                    break;
                }
                case PieceType::Queen: {
                    dir_x = target_y != king_y ? 0 : (target_x > king_x ? -1 : 1);
                    dir_y = target_x != king_x ? 0 : (target_y > king_y ? -1 : 1);

                    if (target_x != king_x && target_y != king_y) {
                        auto difference_x = (int)king_x - (int)target_x;
                        auto difference_y = (int)king_y - (int)target_y;

                        if (abs(difference_x) != abs(difference_y)) {
                            continue;
                        }

                        dir_x = target_x > king_x ? -1 : 1;
                        dir_y = target_y > king_y ? -1 : 1;
                    }

                    break;
                }
                default:
                    continue;;
                }

                auto current_x = target_x;
                auto current_y = target_y;

                auto has_line_of_sight = false;
                std::optional<std::string> potential_pin = std::nullopt;

                current_x += dir_x;
                current_y += dir_y;
                auto temp = Coordinates::create(current_x, current_y);

                while (temp.has_value()) {
                    auto temp_index = temp.value().y() * constants::board_width + temp.value().x();
                    auto temp_target = board.pieces().at(temp_index);

                    current_x += dir_x;
                    current_y += dir_y;
                    temp = Coordinates::create(current_x, current_y);

                    if (temp_target.team() == team) {
                        if (temp_target.type() == PieceType::King) {
                            has_line_of_sight = true;
                            break;
                        }
                        else {
                            if (!potential_pin.has_value()) {
                                potential_pin = coords_to_string_array.at(temp_index);
                                continue;
                            }

                            if (potential_pin.has_value()) {
                                break;
                            }
                        }
                    }
                    else if (temp_target.team() == opponent) {
                        break;
                    }

                    if ((dir_x > 0 && current_x > king_x) || (dir_x < 0 && current_x < king_x) || (dir_y > 0 && current_y > king_y) || (dir_y < 0 && current_y < king_y)) {
                        break;
                    }
                }

                if (has_line_of_sight && potential_pin.has_value()) {
                    result.insert(potential_pin.value());
                }
            }
        }
    }

    return result;
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

    // Generate the DangerZone of our opponent's moves.
    for (int y = 0; y < constants::board_height; ++y) {
        for (int x = 0; x < constants::board_width; ++x) {
            auto attacker = board.pieces().at(y * constants::board_width + x);

            if (attacker.team() == team) {
                continue;
            }

            switch (attacker.type()) {
            case PieceType::Pawn: {
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

                break;
            }
            case PieceType::Knight: {
                auto coords_to_index = [](unsigned int x, unsigned int y) {
                    return y * constants::board_width + x;
                };
                auto setup = [&](unsigned int x, unsigned int y) {
                    if (coords_are_valid(x, y)) {
                        auto index = coords_to_index(x, y);

                        result[index] = true;
                    }
                };

                setup(x + 1, y - 2);
                setup(x + 2, y - 1);
                setup(x + 2, y + 1);
                setup(x + 1, y + 2);
                setup(x - 1, y + 2);
                setup(x - 2, y + 1);
                setup(x - 2, y - 1);
                setup(x - 1, y - 2);

                break;
            }
            case PieceType::Bishop: {
                walk_dangerously(board, result, x, y, 1, -1, attacker.team());
                walk_dangerously(board, result, x, y, 1, 1, attacker.team());
                walk_dangerously(board, result, x, y, -1, 1, attacker.team());
                walk_dangerously(board, result, x, y, -1, -1, attacker.team());

                break;
            }
            case PieceType::Rook: {
                walk_dangerously(board, result, x, y, 0, -1, attacker.team());
                walk_dangerously(board, result, x, y, 1, 0, attacker.team());
                walk_dangerously(board, result, x, y, 0, 1, attacker.team());
                walk_dangerously(board, result, x, y, -1, 0, attacker.team());

                break;
            }
            case PieceType::Queen: {
                walk_dangerously(board, result, x, y, 1, -1, attacker.team());
                walk_dangerously(board, result, x, y, 1, 1, attacker.team());
                walk_dangerously(board, result, x, y, -1, 1, attacker.team());
                walk_dangerously(board, result, x, y, -1, -1, attacker.team());
                walk_dangerously(board, result, x, y, 0, -1, attacker.team());
                walk_dangerously(board, result, x, y, 1, 0, attacker.team());
                walk_dangerously(board, result, x, y, 0, 1, attacker.team());
                walk_dangerously(board, result, x, y, -1, 0, attacker.team());

                break;
            }
            case PieceType::King: {
                auto coords_to_index = [](unsigned int x, unsigned int y) {
                    return y * constants::board_width + x;
                };
                auto setup = [&](unsigned int x, unsigned int y) {
                    if (coords_are_valid(x, y)) {
                        auto index = coords_to_index(x, y);

                        result[index] = true;
                    }
                };

                setup(x + 1, y - 1);
                setup(x + 1, y);
                setup(x + 1, y + 1);
                setup(x, y + 1);
                setup(x - 1, y + 1);
                setup(x - 1, y);
                setup(x - 1, y - 1);
                setup(x, y - 1);

                break;
            }
            default:
                break;
            }
        }
    }

    return result;
}

std::pair<std::set<std::string>, DangerZone> find_attackers(const Board& board, const DangerZone& danger_zone, unsigned int x, unsigned int y)
{
    std::pair<std::set<std::string>, DangerZone> result;

    int index = y * constants::board_width + x;
    auto target = board.pieces().at(index);

    if (target.type() == PieceType::None || !danger_zone[index]) {
        return result;
    }

    auto target_x = index % constants::board_width;
    auto target_y = index / constants::board_width;
    auto coords_as_string = coords_to_string_array.at(index);
    auto team = target.team();
    auto opponent = team == Team::White ? Team::Black : Team::White;

    // Iterate over all the pieces and find if an opponent has a line of sight to the target.
    std::vector<unsigned int> indices;

    for (int y = 0; y < constants::board_height; ++y) {
        for (int x = 0; x < constants::board_width; ++x) {
            int temp_index = y * constants::board_width + x;
            auto temp_target = board.pieces().at(temp_index);
            auto temp_target_x = temp_index % constants::board_width;
            auto temp_target_y = temp_index / constants::board_width;

            if (temp_target.team() != opponent) {
                continue;
            }

            std::optional<MoveSet> move_set = std::nullopt;

            switch (temp_target.type()) {
            case PieceType::Pawn: {
                if (temp_target_x == target_x) {
                    continue;
                }
                if (abs(target_x - temp_target_x) > 1) {
                    continue;
                }

                auto temp_dir = team == Team::White ? 1 : - 1;

                if (temp_target_y + temp_dir != target_y) {
                    continue;
                }

                move_set = generate_pawn_moves(board, temp_target_x, temp_target_y);

                break;
            }
            case PieceType::Knight: {
                move_set = generate_knight_moves(board, temp_target_x, temp_target_y);
                break;
            }
            case PieceType::Bishop: {
                auto difference_x = target_x - temp_target_x;
                auto difference_y = target_y - temp_target_y;

                if (abs(difference_x) != abs(difference_y)) {
                    continue;
                }

                move_set = generate_bishop_moves(board, temp_target_x, temp_target_y);

                break;
            }
            case PieceType::Rook: {
                if (temp_target_x != target_x && temp_target_y != target_y) {
                    continue;
                }

                move_set = generate_rook_moves(board, temp_target_x, temp_target_y);

                break;
            }
            case PieceType::Queen: {
                auto difference_x = target_x - temp_target_x;
                auto difference_y = target_y - temp_target_y;

                if (abs(difference_x) == abs(difference_y)) {
                    move_set = generate_queen_moves(board, temp_target_x, temp_target_y);
                    break;
                }

                if (temp_target_x == target_x || temp_target_y == target_y) {
                    move_set = generate_queen_moves(board, temp_target_x, temp_target_y);
                    break;
                }

                continue;
            }
            case PieceType::King: {
                move_set = generate_king_moves(board, temp_target_x, temp_target_y);
                break;
            }
            default:
                continue;
            }

            if (!move_set.has_value()) {
                continue;
            }

            for (const auto& move : move_set.value()) {
                if (move.substr(2, 2) == coords_as_string) {
                    result.first.insert(move.substr(0, 2));

                    int x = (char)move.substr(0, 1).c_str()[0] - 'a';
                    int y = constants::board_height - std::stoi(move.substr(1, 1));

                    indices.push_back(y * constants::board_width + x);
                }
            }
        }
    }

    // Iterate over the attackers and create a DangerZone that only includes the squares that their line of sight covers.
    for (int i = 0; i < (int)indices.size(); ++i) {
        int index = indices.at(i);
        auto temp_target = board.pieces().at(index);
        auto temp_target_x = index % constants::board_width;
        auto temp_target_y = index / constants::board_width;

        auto dir_x = 0;
        auto dir_y = 0;

        switch (temp_target.type()) {
        case PieceType::Bishop: {
            dir_x = temp_target_x > target_x ? -1 : 1;
            dir_y = temp_target_y > target_y ? -1 : 1;

            break;
        }
        case PieceType::Rook: {
            dir_x = temp_target_y != target_y ? 0 : (temp_target_x > target_x ? -1 : 1);
            dir_y = temp_target_x != target_x ? 0 : (temp_target_y > target_y ? -1 : 1);

            break;
        }
        case PieceType::Queen: {
            dir_x = temp_target_y != target_y ? 0 : (temp_target_x > target_x ? -1 : 1);
            dir_y = temp_target_x != target_x ? 0 : (temp_target_y > target_y ? -1 : 1);

            if (temp_target_x != target_x && temp_target_y != target_y) {
                dir_x = temp_target_x > target_x ? -1 : 1;
                dir_y = temp_target_y > target_y ? -1 : 1;
            }

            break;
        }
        default:
            continue;
        }

        auto current_x = temp_target_x;
        auto current_y = temp_target_y;

        current_x += dir_x;
        current_y += dir_y;
        auto temp_coords = Coordinates::create(current_x, current_y);

        while (temp_coords.has_value()) {
            auto temp_temp_index = temp_coords.value().y() * constants::board_width + temp_coords.value().x();
            auto temp_temp_target = board.pieces().at(temp_temp_index);

            if (temp_temp_target.type() == PieceType::None) {
                result.second.at(temp_temp_index) = true;
            }

            if (current_x == target_x && current_y == target_y) {
                break;
            }

            current_x += dir_x;
            current_y += dir_y;
            temp_coords = Coordinates::create(current_x, current_y);
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
        auto king_side = board.current_team() == Team::White ? CastlingRights::WhiteKingSide : CastlingRights::BlackKingSide;
        auto king_side_index = get_significant_rook_index(king_side).value();

        if (start_index == king_side_index) {
            castling_rights = remove_castling_right(castling_rights, king_side);
        }

        auto queen_side = board.current_team() == Team::White ? CastlingRights::WhiteQueenSide : CastlingRights::BlackQueenSide;
        auto queen_side_index = get_significant_rook_index(queen_side).value();

        if (start_index == queen_side_index) {
            castling_rights = remove_castling_right(castling_rights, queen_side);
        }
    }

    {
        // Capturing a rook on either corner should disable castling on that side.
        auto king_side = current_team == Team::White ? CastlingRights::WhiteKingSide : CastlingRights::BlackKingSide;
        auto king_side_rook_index = get_significant_rook_index(king_side).value();

        if (end_index == king_side_rook_index && contains_castling_right(castling_rights, king_side)) {
            castling_rights = remove_castling_right(castling_rights, king_side);
        }

        auto queen_side = current_team == Team::White ? CastlingRights::WhiteQueenSide : CastlingRights::BlackQueenSide;
        auto queen_side_rook_index = get_significant_rook_index(queen_side).value();

        if (end_index == queen_side_rook_index && contains_castling_right(castling_rights, queen_side)) {
            castling_rights = remove_castling_right(castling_rights, queen_side);
        }
    }

    // Handle setting up a potential en passant.
    if (previous.type() == PieceType::Pawn && abs(dy) == 2) {
        auto direction = dy > 0 ? 1 : -1;
        auto temp = Coordinates::create(move.start().x(), move.start().y() + direction).value();

        // Only enable en_passant_target if an enemy pawn is in position to capture en passant.
        std::optional<Piece> target_left = std::nullopt;
        std::optional<Piece> target_right = std::nullopt;

        auto potential_problems = 0;

        if (temp.x() > 0) {
            target_left = board.pieces().at(end_index - 1);
        }
        if (temp.x() < constants::board_width - 1) {
            target_right = board.pieces().at(end_index + 1);
        }

        if (target_left.has_value()) {
            if (target_left->team() == current_team && target_left->type() == PieceType::Pawn) {
                en_passant_target = coords_to_string_array.at(temp.y() * constants::board_width + temp.x());
                potential_problems += 1;
            }
        }
        if (target_right.has_value()) {
            if (target_right->team() == current_team && target_right->type() == PieceType::Pawn) {
                en_passant_target = coords_to_string_array.at(temp.y() * constants::board_width + temp.x());
                potential_problems += 1;
            }
        }

        // Taking en passant could lead to a discovered check; we need to make sure that cannot happen.
        if (potential_problems == 1) {
            std::optional<unsigned int> king_index = std::nullopt;
            std::vector<Piece> board_slice;

            auto y = current_team == Team::White ? 3 : 4;

            for (int x = 0; x < constants::board_width; ++x) {
                auto target = board.pieces().at(y * constants::board_width + x);

                if (target.team() == current_team && target.type() == PieceType::King) {
                    king_index = y * constants::board_width + x;
                }

                board_slice.push_back(target);
            }

            if (king_index.has_value()) {
                // Remove pawn from board slice (en passant).
                auto x = end_index % constants::board_width;

                if (x < constants::board_width - 1) {
                    auto en_passant_victim = board_slice.at(x + 1);

                    if (en_passant_victim.team() == current_team && en_passant_victim.type() == PieceType::Pawn) {
                        board_slice.at(x + 1) = Piece();
                    }
                }
                if (x > 0) {
                    auto en_passant_victim = board_slice.at(x - 1);

                    if (en_passant_victim.team() == current_team && en_passant_victim.type() == PieceType::Pawn) {
                        board_slice.at(x - 1) = Piece();
                    }
                }

                // Get direction to walk King in.
                auto king_x = king_index.value() % constants::board_width;
                auto dir_x = x > king_x ? 1 : -1;

                king_x += dir_x;

                // Walk King and check if a Rook or Queen is in its line of sight.
                auto danger = false;

                while (king_x < constants::board_width) {
                    auto temp = board_slice.at(king_x);

                    if (temp.team() == board.current_team()) {
                        if (temp.type() == PieceType::Rook || temp.type() == PieceType::Queen) {
                            danger = true;
                        }
                        break;
                    }
                    if (temp.team() == current_team) {
                        break;
                    }

                    king_x += dir_x;
                }

                // Taking en passant would have resulted in a discovered check; en_passant_target should be disabled.
                if (danger) {
                    en_passant_target = std::nullopt;
                }
            }
        }
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

    auto king_coords = coords_to_string_array.at(king_index.value());
    auto king_file = king_coords.substr(0, 1);
    auto king_rank = king_coords.substr(1, 1);
    auto king_x = king_index.value() % constants::board_width;
    auto king_y = king_index.value() / constants::board_width;

    auto opponent = team == Team::White ? Team::Black : Team::White;

    auto moves = generate_move_candidates(board, team);
    auto danger_zone = generate_danger_zone(board, team);
    auto pins = find_pins(board, team).value();
    auto attackers = find_attackers(board, danger_zone, king_x, king_y);

    std::queue<std::string> move_deletion;

    for (auto& pair : moves) {
        auto attacker_coords = coords_to_string_array.at(pair.first);

        // Deal with a pin.
        if (pins.count(attacker_coords) > 0) {
            // If the King is under attacked then we cannot move a pinned piece at all.
            if (danger_zone[king_index.value()]) {
                pair.second.clear();
                continue;
            }

            auto target = board.pieces().at(pair.first);

            auto x = pair.first % constants::board_width;
            auto y = pair.first / constants::board_width;

            switch (target.type()) {
            case PieceType::Pawn: {
                // If my x == king's x then I can only move up
                // If my y == king's y then I cannot move at all
                // Otherwise then I can only move if I am capturing the attacker...

                if (x != king_x && y != king_y) {
                    if (target.team() == Team::White) {
                        if (y > king_y) {
                            pair.second.clear();
                            continue;
                        }

                        auto temp_dir = x > king_x ? 1 : -1;
                        auto temp_index = (y - 1) * constants::board_width + (x + temp_dir);

                        auto temp_target = board.pieces().at(temp_index);

                        if (temp_target.team() == opponent) {
                            auto temp_coords = coords_to_string_array.at(temp_index);

                            for (const auto& move : pair.second) {
                                if (move.substr(2, 2) != temp_coords) {
                                    move_deletion.push(move);
                                }
                            }
                            break;
                        }
                        else {
                            pair.second.clear();
                            continue;
                        }
                    }
                    else if (target.team() == Team::Black) {
                        if (y < king_y) {
                            pair.second.clear();
                            continue;
                        }

                        auto temp_dir = x > king_x ? 1 : -1;
                        auto temp_index = (y + 1) * constants::board_width + (x + temp_dir);

                        auto temp_target = board.pieces().at(temp_index);

                        if (temp_target.team() == opponent) {
                            auto temp_coords = coords_to_string_array.at(temp_index);

                            for (const auto& move : pair.second) {
                                if (move.substr(2, 2) != temp_coords) {
                                    move_deletion.push(move);
                                }
                            }
                            break;
                        }
                        else {
                            pair.second.clear();
                            continue;
                        }
                    }
                }

                if (y == king_y) {
                    pair.second.clear();
                }

                for (auto& move : pair.second) {
                    auto file = move.substr(2, 1);
                    auto rank = move.substr(3, 1);

                    if (x == king_x && file != king_file) {
                        move_deletion.push(move);
                    }
                }

                break;
            }
            case PieceType::Bishop: {
                // If my x == king's x then I cannot move at all
                // If my y == king's y then I cannot move at all
                // Otherwise then I can only move along the King's diagonal

                if (x == king_x || y == king_y) {
                    pair.second.clear();
                    continue;
                }

                auto dir_x = x > king_x ? 1 : -1;
                auto dir_y = y > king_y ? 1 : -1;

                // Get all coords from King to my position.
                std::set<std::string> valid_coords;

                auto start_x = king_x;
                auto start_y = king_y;

                while (coords_are_valid(start_x + dir_x, start_y + dir_y)) {
                    start_x += dir_x;
                    start_y += dir_y;

                    auto temp_index = start_y * constants::board_width + start_x;

                    valid_coords.insert(coords_to_string_array.at(temp_index));
                }

                // Discard any moves that are outside of the King's diagonal.
                for (const auto& move : pair.second) {
                    if (valid_coords.count(move.substr(2, 2)) == 0) {
                        move_deletion.push(move);
                    }
                }

                break;
            }
            case PieceType::Rook: {
                // If my x == king's x then I cannot move in the y direction
                // If my y == king's y then I cannot move in the x direction
                // Otherwise I cannot move at all

                if (x != king_x && y != king_y) {
                    pair.second.clear();
                    continue;
                }

                for (auto& move : pair.second) {
                    auto file = move.substr(2, 1);
                    auto rank = move.substr(3, 1);

                    if ((x == king_x && file != king_file) || (y == king_y && rank != king_rank)) {
                        move_deletion.push(move);
                    }
                }

                break;
            }
            case PieceType::Queen: {
                // If my x == king's x then I cannot move in the y direction
                // If my y == king's y then I cannot move in the x direction
                // Otherwise then I can only move along the king's diagonal

                if (x == king_x || y == king_y) {
                    for (auto& move : pair.second) {
                        auto file = move.substr(2, 1);
                        auto rank = move.substr(3, 1);

                        if ((x == king_x && file != king_file) || (y == king_y && rank != king_rank)) {
                            move_deletion.push(move);
                        }
                    }
                    break;
                }

                auto dir_x = x > king_x ? 1 : -1;
                auto dir_y = y > king_y ? 1 : -1;

                // Get all coords from King to my position.
                std::set<std::string> valid_coords;

                auto start_x = king_x;
                auto start_y = king_y;

                while (coords_are_valid(start_x + dir_x, start_y + dir_y)) {
                    start_x += dir_x;
                    start_y += dir_y;

                    auto temp_index = start_y * constants::board_width + start_x;

                    valid_coords.insert(coords_to_string_array.at(temp_index));
                }

                // Discard any moves that are outside of the King's diagonal.
                for (const auto& move : pair.second) {
                    if (valid_coords.count(move.substr(2, 2)) == 0) {
                        move_deletion.push(move);
                    }
                }

                break;
            }
            default:
                // Knights should not be able to move at all.
                pair.second.clear();
                break;
            }

            while (move_deletion.size() > 0) {
                auto target = move_deletion.front();
                pair.second.erase(target);
                move_deletion.pop();
            }

            continue;
        }

        if (pair.first == king_index.value()) {
            for (const auto& move : pair.second) {
                if (castle_set.count(move) > 0) {
                    // If the King is in check then make sure it cannot castle.
                    if (danger_zone[pair.first]) {
                        move_deletion.push(move);
                        continue;
                    }

                    // Make sure the King can not castle through or into a check.
                    auto king_side = team == Team::White ? CastlingRights::WhiteKingSide : CastlingRights::BlackKingSide;
                    auto queen_side = team == Team::White ? CastlingRights::WhiteQueenSide : CastlingRights::BlackQueenSide;
                    auto king_side_move = team == Team::White ? "e1g1" : "e8g8";
                    auto queen_side_move = team == Team::White ? "e1c1" : "e8c8";

                    auto x = pair.first % constants::board_width;
                    auto y = pair.first / constants::board_width;

                    // This does not explicitly check if x or y is out of bounds; however, verifying the board's CastlingRights beforehand
                    // guarantees the index will be valid.
                    auto coords_to_index = [](unsigned int x, unsigned int y) {
                        return y * constants::board_width + x;
                    };

                    if (contains_castling_right(board.castling_rights(), king_side) && move == king_side_move) {
                        if (danger_zone[coords_to_index(x + 1, y)] || danger_zone[coords_to_index(x + 2, y)]) {
                            move_deletion.push(move);
                        }
                    }

                    if (contains_castling_right(board.castling_rights(), queen_side) && move == queen_side_move) {
                        if (danger_zone[coords_to_index(x - 1, y)] || danger_zone[coords_to_index(x - 2, y)]) {
                            move_deletion.push(move);
                        }
                    }

                    continue;
                }

                auto new_king_index = coords_to_uint(move.substr(2, 2));

                if (danger_zone[new_king_index]) {
                    move_deletion.push(move);
                }
            }
        }

        if (pair.first != king_index) {
            if (!danger_zone[king_index.value()]) {
                continue;
            }

            auto pawn_can_attack_en_passant = false;

            if (board.en_passant_target().has_value()) {
                for (const auto& attacker : attackers.first) {
                    auto temp = coords_to_uint(attacker);
                    auto target = board.pieces().at(temp);

                    if (target.type() == PieceType::Pawn) {
                        pawn_can_attack_en_passant = true;
                        break;
                    }
                }
            }

            if (attackers.first.size() > 1) {
                pair.second.clear();
            }
            else {
                for (const auto& move : pair.second) {
                    auto end = move.substr(2, 2);

                    if (attackers.first.count(end) > 0 || attackers.second[coords_to_uint(end)]) {
                        continue;
                    }

                    if (end == board.en_passant_target() && pawn_can_attack_en_passant) {
                        auto target = board.pieces().at(pair.first);

                        if (target.type() == PieceType::Pawn) {
                            continue;
                        }
                    }

                    move_deletion.push(move);
                }
            }
        }

        while (move_deletion.size() > 0) {
            auto target = move_deletion.front();
            pair.second.erase(target);
            move_deletion.pop();
        }
    }

    auto no_moves = !can_move(moves);
    auto king_location = Coordinates::create(king_index.value() % constants::board_width, king_index.value() / constants::board_width).value();
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
