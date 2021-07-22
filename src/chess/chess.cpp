#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <iostream>
#include <memory>
#include "chess.hpp"
#include "coordinates.hpp"
#include "../dam/graphics.hpp"
#include "../dam/input.hpp"
#include "../dam/palette.hpp"
#include "../dam/window.hpp"

int random_range(int min, int max)
{
    return min + rand() % (max - min);
}

std::optional<CastlingRights> lan_as_castling_rights(std::string lan)
{
    // This does not check whether or not the current piece is even a King!

    if (lan == "e1g1") {
        return CastlingRights::WhiteKingSide;
    }
    if (lan == "e1c1") {
        return CastlingRights::WhiteQueenSide;
    }
    if (lan == "e8g8") {
        return CastlingRights::BlackKingSide;
    }
    if (lan == "e8c8") {
        return CastlingRights::BlackQueenSide;
    }

    return std::nullopt;
}

void Chess::handle_resize(dam::Context& ctx)
{
    square_size = dam::window::get_height(ctx) / constants::board_height;

    if (square_size * constants::board_width > (int)dam::window::get_width(ctx)) {
        square_size = dam::window::get_width(ctx) / constants::board_width;
    }

    board_offset = dam::Vector2F(
                       (dam::window::get_width(ctx) - (square_size * constants::board_width)) / 2,
                       (dam::window::get_height(ctx) - (square_size * constants::board_height)) / 2
                   );

    sprite_scale = (float)square_size / sprite_size;

    dam::graphics::unload_font(font);
    font = dam::graphics::load_font("./content/fonts/FiraCode-SemiBold.ttf", square_size * 0.22);
}

void Chess::reset_selection()
{
    selected = false;
    initial_selection = std::nullopt;
}

void Chess::queue_move(std::string lan)
{
    auto move = Move::create(lan);

    if (!move.has_value()) {
        return;
    }

    move_was_queued = true;
    queued_move = move;

    primary_renderable.set(dam::Vector2F(move->start().x(), move->start().y()));

    auto start = dam::Vector2F(move->start().x(), move->start().y());
    auto end = dam::Vector2F(move->end().x(), move->end().y());

    primary_mover = Tweenable(start, end, move_duration);
}

void Chess::queue_castle(std::string lan, CastlingRights castling_rights)
{
    auto move = Move::create(lan);

    if (!move.has_value()) {
        return;
    }

    queued_move = move.value();

    dam::Vector2F king_start;
    dam::Vector2F king_end;
    dam::Vector2F rook_start;
    dam::Vector2F rook_end;

    switch (castling_rights) {
    case CastlingRights::WhiteKingSide: {
        king_start = dam::Vector2F(4, 7);
        king_end = dam::Vector2F(6, 7);
        rook_start = dam::Vector2F(7, 7);
        rook_end = dam::Vector2F(5, 7);
        break;
    }
    case CastlingRights::WhiteQueenSide: {
        king_start = dam::Vector2F(4, 7);
        king_end = dam::Vector2F(2, 7);
        rook_start = dam::Vector2F(0, 7);
        rook_end = dam::Vector2F(3, 7);
        break;
    }
    case CastlingRights::BlackKingSide: {
        king_start = dam::Vector2F(4, 0);
        king_end = dam::Vector2F(6, 0);
        rook_start = dam::Vector2F(7, 0);
        rook_end = dam::Vector2F(5, 0);
        break;
    }
    case CastlingRights::BlackQueenSide: {
        king_start = dam::Vector2F(4, 0);
        king_end = dam::Vector2F(2, 0);
        rook_start = dam::Vector2F(0, 0);
        rook_end = dam::Vector2F(3, 0);
        break;
    }
    default:
        break;
    }

    move_was_queued = true;

    primary_renderable.set(king_start);

    primary_mover = Tweenable(king_start, king_end, move_duration);
    secondary_mover = Tweenable(rook_start, rook_end, move_duration);
    rook_index = rook_start.y() * constants::board_width + rook_start.x();
}

void Chess::update_input(dam::Context& ctx)
{
    using namespace dam::input;

    if (Keyboard::pressed(ctx, Key::Escape)) {
        loop = false;
    }

    if (Keyboard::pressed(ctx, Key::F)) {
        board_flipped = !board_flipped;
    }

    if (Mouse::get_scroll_stride(ctx) > 0) {
        match.redo();
        reset_selection();
    }
    if (Mouse::get_scroll_stride(ctx) < 0) {
        match.undo();
        reset_selection();
    }

    if (selected && Mouse::pressed(ctx, MouseButton::Right)) {
        reset_selection();
    }

    if (move_was_queued) {
        return;
    }

    if (Mouse::pressed(ctx, MouseButton::Left)) {
        if (!selected) {
            auto position = Mouse::get_position(ctx);
            auto x = (int)((position.x() - board_offset.x()) / square_size);
            auto y = (int)((position.y() - board_offset.y()) / square_size);

            if (board_flipped) {
                x = constants::board_width - x - 1;
                y = constants::board_height - y - 1;
            }

            auto target = match.board().get(x, y);

            if (!target.has_value() || target->team() != match.board().current_team()) {
                return;
            }

            initial_selection = Coordinates::create(x, y);
            selected = true;
        }
        else {
            auto position = Mouse::get_position(ctx);
            auto x = (int)((position.x() - board_offset.x()) / square_size);
            auto y = (int)((position.y() - board_offset.y()) / square_size);

            if (board_flipped) {
                x = constants::board_width - x - 1;
                y = constants::board_height - y - 1;
            }

            auto target = match.board().get(x, y);

            if (!target.has_value()) {
                return;
            }

            // Quickly change to another piece.
            if (target->team() == match.board().current_team()) {
                initial_selection = Coordinates::create(x, y);
                return;
            }

            auto coords = Coordinates::create(x, y).value();
            auto lan = initial_selection->to_string() + coords.to_string();

            auto initial_piece = match.board().get(initial_selection->x(), initial_selection->y()).value();

            if (initial_piece.type() == PieceType::Pawn) {
                auto start_y = 0;
                auto end_y = 0;

                switch (match.board().current_team()) {
                case Team::White: {
                    start_y = 1;
                    end_y = 0;
                    break;
                }
                case Team::Black: {
                    start_y = 6;
                    end_y = 7;
                    break;
                }
                default:
                    break;
                }

                // Check if we are about to promote a pawn.
                if ((int)initial_selection->y() == start_y && y == end_y) {
                    // TODO(thismarvin): Instead of automatically promoting to a queen, implement a UI that allows the player to choose what piece to promote to.
                    lan += "q";
                }
            }

            if (match.analysis().contains_move(lan)) {
                // Castling requires that two pieces move simultaneously; however, `queue_move` only supports moving one piece.
                // The following makes sure we use `queue_castling` (which supports moving two pieces) instead of `queue_move` when necessary.
                if (initial_piece.type() == PieceType::King) {
                    auto temp = lan_as_castling_rights(lan);
                    if (temp.has_value()) {
                        queue_castle(lan, temp.value());
                        return;
                    }
                }

                queue_move(lan);
            }
            else {
                reset_selection();
            }
        }
    }
}

void Chess::update_ai(dam::Context& ctx)
{
    if (move_was_queued) {
        return;
    }

    if (!match.at_end() || match.board().current_team() != ai_team) {
        return;
    }

    if (gm::end_condition(match.analysis().king_safety())) {
        return;
    }

    engine_delay_timer += ctx.delta_time;

    if (engine_delay_timer >= ai_delay_duration) {
        // Random AI
        std::vector<unsigned int> potential_pieces;
        for (auto const& pair : match.analysis().moves()) {
            potential_pieces.push_back(pair.first);
        }

        std::optional<unsigned int> piece_index = std::nullopt;
        std::optional<std::string> move = std::nullopt;

        while (!move.has_value()) {
            piece_index = potential_pieces[random_range(0, (int)potential_pieces.size())];
            MoveSet move_set = match.analysis().moves().at(piece_index.value());

            if (move_set.size() == 0) {
                continue;
            }

            int temp = random_range(0, (int)move_set.size());

            int i = 0;
            for (const auto& entry : move_set) {
                if (i == temp) {
                    move = entry;
                    break;
                }

                i += 1;
            }
        }

        queue_move(move.value());

        engine_delay_timer = 0;
    }
}

void Chess::update_tweening(dam::Context& ctx)
{
    if (!move_was_queued) {
        return;
    }

    primary_mover.update(ctx.delta_time);
    primary_renderable.set(primary_mover.position());

    if (secondary_mover.has_value()) {
        secondary_mover->update(ctx.delta_time);
        secondary_renderable.set(secondary_mover->position());
    }

    if (primary_mover.done()) {
        move_was_queued = false;
        match.submit_move(queued_move->lan());
        reset_selection();
        secondary_mover = std::nullopt;
    }
}

dam::Vector2F Chess::calculate_draw_position(float x, float y)
{
    auto draw_x = x;
    auto draw_y = y;

    if (board_flipped) {
        draw_x = constants::board_width - draw_x - 1;
        draw_y = constants::board_height - draw_y - 1;
    }

    draw_x = draw_x * square_size + board_offset.x();
    draw_y = draw_y * square_size + board_offset.y();

    return dam::Vector2F(draw_x, draw_y);
}

std::optional<dam::graphics::ImageRegion> Chess::image_region_from_piece(Piece piece)
{
    auto subregion_x = 0;

    switch (piece.type()) {
    case PieceType::Pawn:
        break;
    case PieceType::Bishop:
        subregion_x = sprite_size;
        break;
    case PieceType::Knight:
        subregion_x = sprite_size * 2;
        break;
    case PieceType::Rook:
        subregion_x = sprite_size * 3;
        break;
    case PieceType::Queen:
        subregion_x = sprite_size * 4;
        break;
    case PieceType::King:
        subregion_x = sprite_size * 5;
        break;
    case PieceType::None:
        return std::nullopt;
    }

    return dam::graphics::ImageRegion(
               subregion_x,
               piece.team() == Team::White ? 0 : sprite_size,
               sprite_size,
               sprite_size
           );
}

void Chess::draw_board(dam::Context& ctx)
{
    for (int y = 0; y < constants::board_height; ++y) {
        for (int x = 0; x < constants::board_width; ++x) {
            auto color = (x + y) % 2 == 0 ? board_light_color : board_dark_color;

            auto params = dam::graphics::DrawParams()
                          .set_position(board_offset.x() + x * square_size, board_offset.y() + y * square_size)
                          .set_scale(square_size, square_size)
                          .set_tint(color);

            draw_rectangle(ctx, params);
        }
    }
}

void Chess::draw_coordinates(dam::Context& ctx)
{
    for (int x = 0; x < constants::board_width; ++x) {
        auto color = x % 2 == 0 ? board_light_color : board_dark_color;
        auto temp = !board_flipped ? 'a' + x : 'a' + (constants::board_width - 1 - x);
        std::string text = "";
        text.push_back(temp);

        auto params = dam::graphics::DrawParams()
                      .set_position(board_offset.x() + x * square_size + square_size * 0.02, board_offset.y() + (constants::board_height - 1) * square_size + square_size * 0.76)
                      .set_tint(color);

        draw_text(ctx, text, font, params);
    }
    for (int y = 0; y < constants::board_height; ++y) {
        auto color = y % 2 == 0 ? board_light_color : board_dark_color;
        auto temp = !board_flipped ? '1' + (constants::board_height - 1 - y) : '1' + y;
        std::string text = "";
        text.push_back(temp);

        auto params = dam::graphics::DrawParams()
                      .set_position(board_offset.x() + (constants::board_width - 1) * square_size + square_size * 0.85, board_offset.y() + y * square_size - square_size * 0.01)
                      .set_tint(color);

        draw_text(ctx, text, font, params);
    }
}

void Chess::draw_recent_move_indicator(dam::Context& ctx)
{
    using namespace dam::graphics;

    auto start_x = match.last_move().start().x();
    auto start_y = match.last_move().start().y();
    auto end_x = match.last_move().end().x();
    auto end_y = match.last_move().end().y();

    if (move_was_queued) {
        start_x = queued_move->start().x();
        start_y = queued_move->start().y();
        end_x = queued_move->end().x();
        end_y = queued_move->end().y();
    }

    // This is sort of a hack to get around drawing a nullmove...
    if (start_x == end_x && start_y == end_y) {
        return;
    }

    auto start_draw_position = calculate_draw_position(start_x, start_y);
    auto end_draw_position = calculate_draw_position(end_x, end_y);

    auto start_params = DrawParams()
                        .set_position(start_draw_position.x(), start_draw_position.y())
                        .set_scale(square_size, square_size)
                        .set_tint(Color(0xffff00, 0.3));
    auto end_params = DrawParams()
                      .set_position(end_draw_position.x(), end_draw_position.y())
                      .set_scale(square_size, square_size)
                      .set_tint(Color(0xffff00, 0.3));

    draw_rectangle(ctx, start_params);
    draw_rectangle(ctx, end_params);
}

// Draw an indicator around the King when they are being attacked.
void Chess::draw_king_safety(dam::Context& ctx)
{
    // TODO(thismarvin): This is so ugly... can someone make this a method or something?
    if (match.analysis().king_safety() != gm::KingSafety::Check && match.analysis().king_safety() != gm::KingSafety::Checkmate) {
        return;
    }

    auto x = match.analysis().king_location().x();
    auto y = match.analysis().king_location().y();

    auto draw_position = calculate_draw_position(x, y);

    auto temp_params = dam::graphics::DrawParams()
                       .set_position(draw_position.x(), draw_position.y())
                       .set_scale(square_size, square_size)
                       .set_tint(dam::graphics::Color(0xff0000, 0.4));

    draw_rectangle_but_not_filled(ctx, temp_params, square_size * 0.25);
}

void Chess::draw_selection(dam::Context& ctx)
{
    if (!selected || move_was_queued) {
        return;
    }

    auto draw_position = calculate_draw_position(initial_selection->x(), initial_selection->y());

    auto params = dam::graphics::DrawParams()
                  .set_position(draw_position.x(), draw_position.y())
                  .set_scale(square_size, square_size)
                  .set_tint(dam::graphics::Color(0x547c64, 0.8));

    draw_rectangle(ctx, params);
}

// Draw an indicator for every possible square the current piece can move to.
void Chess::draw_moves(dam::Context& ctx)
{
    using namespace dam::graphics;

    if (!initial_selection.has_value() || move_was_queued) {
        return;
    }

    auto index = initial_selection->y() * constants::board_width + initial_selection->x();
    auto target_move_set = match.analysis().moves().at(index);

    auto mouse_position = dam::input::Mouse::get_position(ctx);
    auto mouse_x = (int)((mouse_position.x() - board_offset.x()) / square_size);
    auto mouse_y = (int)((mouse_position.y() - board_offset.y()) / square_size);

    std::optional<int> duplicate = std::nullopt;

    for (const auto& value : target_move_set) {
        auto temp_coords = value.substr(2, 2);
        auto x = temp_coords.c_str()[0] - 'a';
        auto y = constants::board_height - std::stoi(temp_coords.substr(1, 1));

        // The following is necessary in order to only draw one move indicator instead of one for each promotion.
        // I will admit the solution is hacky, but it works!
        auto temp = y * constants::board_width + x;

        if (duplicate == temp) {
            continue;
        }

        duplicate = temp;

        // This could be simplified with calculate_draw_position, but I do not know how to handle to mouse_hover part...
        auto temp_draw_x = x;
        auto temp_draw_y = y;

        if (board_flipped) {
            temp_draw_x = constants::board_width - x - 1;
            temp_draw_y = constants::board_height - y - 1;
        }

        auto mouse_hover = mouse_x == temp_draw_x && mouse_y == temp_draw_y;

        temp_draw_x = temp_draw_x * square_size + board_offset.x();
        temp_draw_y = temp_draw_y * square_size + board_offset.y();

        auto target_piece = match.board().get(x, y);

        if (target_piece.has_value() && target_piece->team() == Team::None) {
            auto radius = square_size * 0.35 * 0.5;
            auto alpha = 0.8;

            if (mouse_hover) {
                radius *= 1.1;
                alpha = 0.95;
            }

            auto temp_params = DrawParams()
                               .set_position(temp_draw_x + square_size * 0.5 - radius, temp_draw_y + square_size * 0.5 - radius)
                               .set_scale(radius * 2, radius * 2)
                               .set_tint(Color(0x547c64, alpha));

            draw_circle(ctx, temp_params);
        }
        else {
            auto line_width = square_size * 0.25;
            auto alpha = 0.2;

            if (mouse_hover) {
                line_width *= 1.2;
                alpha = 0.3;
            }

            auto temp_params = DrawParams()
                               .set_position(temp_draw_x, temp_draw_y)
                               .set_scale(square_size, square_size)
                               .set_tint(Color(0xff0000, alpha));

            draw_rectangle_but_not_filled(ctx, temp_params, line_width);
        }
    }
}

void Chess::draw_pieces(dam::Context& ctx)
{
    for (int y = 0; y < constants::board_height; ++y) {
        for (int x = 0; x < constants::board_width; ++x) {
            // Do not draw the piece that is currently moving!
            if (move_was_queued) {
                int temp_x = queued_move->start().x();
                int temp_y = queued_move->start().y();

                if (board_flipped) {
                    temp_x = constants::board_width - temp_x - 1;
                    temp_y = constants::board_width - temp_y - 1;
                }

                if (x == temp_x && y == temp_y) {
                    continue;
                }

                if (secondary_mover.has_value()) {
                    auto temp = board_flipped ? constants::board_size - rook_index - 1 : rook_index;

                    if (y * constants::board_width + x == temp) {
                        continue;
                    }
                }
            }

            auto index = board_flipped ? ((constants::board_height - 1) - y) * constants::board_width + ((constants::board_width - 1) - x) : y * constants::board_width + x;
            auto current = match.board().pieces()[index];

            if (current.type() != PieceType::None) {
                auto params = dam::graphics::DrawParams()
                              .set_position(board_offset.x() + x * square_size, board_offset.y() + y * square_size)
                              .set_scale(sprite_scale, sprite_scale);
                auto region = image_region_from_piece(current).value();

                draw_texture(ctx, pieces, region, params);
            }
        }
    }
}

void Chess::draw_move_animation(dam::Context& ctx)
{
    if (!move_was_queued) {
        return;
    }

    if (secondary_mover.has_value()) {
        auto temp = secondary_renderable.interpolate(ctx.alpha);

        auto draw_position = calculate_draw_position(temp.x(), temp.y());

        auto piece = match.board().pieces()[rook_index];

        auto region = image_region_from_piece(piece).value();
        auto params = dam::graphics::DrawParams()
                      .set_position(draw_position.x(), draw_position.y())
                      .set_scale(sprite_scale, sprite_scale);

        draw_texture(ctx, pieces, region, params);
    }

    auto temp = primary_renderable.interpolate(ctx.alpha);

    auto draw_position = calculate_draw_position(temp.x(), temp.y());

    auto temp_x = queued_move->start().x();
    auto temp_y = queued_move->start().y();

    auto index = temp_y * constants::board_width + temp_x;
    auto piece = match.board().pieces()[index];

    auto region = image_region_from_piece(piece).value();
    auto params = dam::graphics::DrawParams()
                  .set_position(draw_position.x(), draw_position.y())
                  .set_scale(sprite_scale, sprite_scale);

    draw_texture(ctx, pieces, region, params);
}

void Chess::initialize(dam::Context& ctx)
{
    pieces = dam::graphics::load_texture("./content/sprites/pieces.png");

    handle_resize(ctx);

    srand(time(NULL));
}

void Chess::update(dam::Context& ctx)
{
    update_input(ctx);
    update_ai(ctx);
    update_tweening(ctx);
}

void Chess::event(dam::Context& ctx, dam::EventType event)
{
    switch (event) {
    case dam::EventType::WindowResize:
        handle_resize(ctx);
        break;
    default:
        break;
    }
}

void Chess::draw(dam::Context& ctx)
{
    using namespace dam::graphics;

    clear(palette::black);

    draw_board(ctx);
    draw_coordinates(ctx);
    draw_recent_move_indicator(ctx);
    draw_king_safety(ctx);
    draw_selection(ctx);
    draw_moves(ctx);
    draw_pieces(ctx);
    draw_move_animation(ctx);
}

void Chess::destroy(dam::Context& ctx)
{
    using namespace dam::graphics;

    unload_font(font);
    unload_texture(pieces);
}
