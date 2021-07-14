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
    move_time = 0;
    current_position = dam::Vector2F(move->start().x(), move->start().y());
}

void Chess::initialize(dam::Context& ctx)
{
    using namespace dam::graphics;

    font = load_font("./content/fonts/FiraCode-SemiBold.ttf", 16);

    pieces = load_texture("./content/sprites/pieces.png");
    sprite_size = 16;

    handle_resize(ctx);

    match = Match::create();

    board_flipped = false;
    selected = false;

    srand(time(NULL));
}

void Chess::update(dam::Context& ctx)
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

    if (!move_was_queued && Mouse::pressed(ctx, MouseButton::Left)) {
        if (!selected) {
            auto position = Mouse::get_position(ctx);
            auto x = (int)((position.x() - board_offset.x()) / square_size);
            auto y = (int)((position.y() - board_offset.y()) / square_size);

            if (board_flipped) {
                x = constants::board_width - x - 1;
                y = constants::board_height - y - 1;
            }

            auto target = match.board().get(x, y);

            if (target.has_value() && target->team() == match.board().current_team()) {
                selected = true;
                initial_selection = Coordinates::create(x, y);
            }
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

            if (target.has_value()) {
                // Quickly change to another piece.
                if (target->team() == match.board().current_team()) {
                    initial_selection = Coordinates::create(x, y);
                }
                else {
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
                        queue_move(lan);
                    }
                    else {
                        reset_selection();
                    }
                }
            }
        }
    }

    if (!move_was_queued && match.at_end() && match.board().current_team() == Team::Black && match.analysis().king_safety() != gm::KingSafety::Checkmate && match.analysis().king_safety() != gm::KingSafety::Stalemate) {
        engine_delay += ctx.delta_time;

        if (engine_delay >= 0.8) {
            // Random AI
            if (match.at_end() && match.board().current_team() == Team::Black) {
                if (match.analysis().king_safety() != gm::KingSafety::Stalemate && match.analysis().king_safety() != gm::KingSafety::Checkmate) {
                    std::vector<int> potential_pieces;
                    for (auto const& pair : match.analysis().moves()) {
                        potential_pieces.push_back(pair.first);
                    }

                    std::optional<unsigned int> piece_index = std::nullopt;
                    std::optional<std::string> move = std::nullopt;

                    while (!move.has_value()) {
                        piece_index = (unsigned int)(potential_pieces[rand() % (int)potential_pieces.size()]);
                        MoveSet move_set = match.analysis().moves().at(piece_index.value());

                        if (move_set.size() == 0) {
                            continue;
                        }

                        int temp = rand() % (int)move_set.size();

                        int i = 0;
                        for (const auto& yeet : move_set) {
                            if (i == temp) {
                                move = yeet;
                                break;
                            }

                            i += 1;
                        }
                    }

                    queue_move(move.value());
                }
            }

            engine_delay = 0;
        }
    }

    if (move_was_queued) {
        int start_x = queued_move->start().x();
        int start_y = queued_move->start().y();
        int end_x = queued_move->end().x();
        int end_y = queued_move->end().y();

        move_time += ctx.delta_time / 0.25;

        auto x = (1 - move_time) * start_x + move_time * end_x;
        auto y = (1 - move_time) * start_y + move_time * end_y;

        previous_position = current_position;
        current_position = dam::Vector2F(x, y);

        if (move_time >= 1) {
            move_was_queued = false;
            match.submit_move(queued_move->lan());
            reset_selection();
        }
    }
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

    auto light_color = Color(0xeeeed2);
    auto dark_color = Color(0x769656);

    // Draw board.
    for (int y = 0; y < constants::board_height; ++y) {
        for (int x = 0; x < constants::board_width; ++x) {
            auto color = (x + y) % 2 == 0 ? light_color : dark_color;

            auto params = DrawParams()
                          .set_position(board_offset.x() + x * square_size, board_offset.y() + y * square_size)
                          .set_scale(square_size, square_size)
                          .set_tint(color);
            draw_rectangle(ctx, params);
        }
    }

    // Draw algebraic notations.
    for (int x = 0; x < constants::board_width; ++x) {
        auto color = x % 2 == 0 ? light_color : dark_color;
        auto temp = !board_flipped ? 'a' + x : 'a' + (constants::board_width - 1 - x);
        std::string text = "";
        text.push_back(temp);

        auto params = DrawParams()
                      .set_position(board_offset.x() + x * square_size + square_size * 0.08, board_offset.y() + (constants::board_height - 1) * square_size + square_size * 0.75)
                      .set_tint(color);
        draw_text(ctx, text, font, params);
    }
    for (int y = 0; y < constants::board_height; ++y) {
        auto color = y % 2 == 0 ? light_color : dark_color;
        auto temp = !board_flipped ? '1' + (constants::board_height - 1 - y) : '1' + y;
        std::string text = "";
        text.push_back(temp);

        auto params = DrawParams()
                      .set_position(board_offset.x() + (constants::board_width - 1) * square_size + square_size * 0.75, board_offset.y() + y * square_size + square_size * 0.08)
                      .set_tint(color);
        draw_text(ctx, text, font, params);
    }

    // Draw an indicator that represents the most recent move.
    if (match.last_move().lan() != Move::nullmove.lan()) {
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

        if (board_flipped) {
            start_x = constants::board_width - start_x - 1;
            start_y = constants::board_height - start_y - 1;
            end_x = constants::board_width - end_x - 1;
            end_y = constants::board_height - end_y - 1;
        }

        start_x = start_x * square_size + board_offset.x();
        start_y = start_y * square_size + board_offset.y();
        end_x = end_x * square_size + board_offset.x();
        end_y = end_y * square_size + board_offset.y();

        auto start_params = DrawParams()
                            .set_position(start_x, start_y)
                            .set_scale(square_size, square_size)
                            .set_tint(Color(0xffff00, 0.3));
        auto end_params = DrawParams()
                          .set_position(end_x, end_y)
                          .set_scale(square_size, square_size)
                          .set_tint(Color(0xffff00, 0.3));

        draw_rectangle(ctx, start_params);
        draw_rectangle(ctx, end_params);
    }

    // Draw an indicator around the King when they are being attacked.
    if (match.analysis().king_safety() == gm::KingSafety::Check || match.analysis().king_safety() == gm::KingSafety::Checkmate) {
        auto draw_x = match.analysis().king_location().x();
        auto draw_y = match.analysis().king_location().y();

        if (board_flipped) {
            draw_x = constants::board_width - draw_x - 1;
            draw_y = constants::board_height - draw_y - 1;
        }

        draw_x = draw_x * square_size + board_offset.x();
        draw_y = draw_y * square_size + board_offset.y();

        auto temp_params = DrawParams()
                           .set_position(draw_x, draw_y)
                           .set_scale(square_size, square_size)
                           .set_tint(Color(0xff0000, 0.4));

        draw_rectangle_but_not_filled(ctx, temp_params, square_size * 0.25);
    }

    // Draw an indicator for every possible square the current piece can move to.
    if (selected && !move_was_queued) {
        auto first_char = initial_selection->to_string().substr(0, 1).c_str()[0];
        auto second_char = initial_selection->to_string().substr(1, 1);
        auto x = first_char - 'a';
        auto y = constants::board_height - std::stoi(second_char);

        auto draw_x = x;
        auto draw_y = y;

        if (board_flipped) {
            draw_x = constants::board_width - draw_x - 1;
            draw_y = constants::board_height - draw_y - 1;
        }

        draw_x = draw_x * square_size + board_offset.x();
        draw_y = draw_y * square_size + board_offset.y();

        auto params = DrawParams()
                      .set_position(draw_x, draw_y)
                      .set_scale(square_size, square_size)
                      .set_tint(Color(0x547c64, 0.8));

        draw_rectangle(ctx, params);

        auto index = y * constants::board_width + x;
        auto moves = match.analysis().moves();
        auto target_move_set = moves.at(index);

        auto mouse_position = dam::input::Mouse::get_position(ctx);
        auto mouse_x = (int)((mouse_position.x() - board_offset.x()) / square_size);
        auto mouse_y = (int)((mouse_position.y() - board_offset.y()) / square_size);

        for (const auto& value : target_move_set) {
            // TODO(thismarvin): This is very similar to the code above. How can we get rid of code duplication?
            auto temp_coords = value.substr(2, 2);
            auto temp_x = temp_coords.c_str()[0] - 'a';
            auto temp_y = constants::board_height - std::stoi(temp_coords.substr(1, 1));

            auto temp_draw_x = temp_x;
            auto temp_draw_y = temp_y;

            if (board_flipped) {
                temp_draw_x = constants::board_width - temp_x - 1;
                temp_draw_y = constants::board_height - temp_y - 1;
            }

            auto mouse_hover = mouse_x == temp_draw_x && mouse_y == temp_draw_y;

            temp_draw_x = temp_draw_x * square_size + board_offset.x();
            temp_draw_y = temp_draw_y * square_size + board_offset.y();

            auto target_piece = match.board().get(temp_x, temp_y);

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

    al_set_target_backbuffer(ctx.display);
    // Draw pieces.
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
            }
            auto index = board_flipped ? ((constants::board_height - 1) - y) * constants::board_width + ((constants::board_width - 1) - x) : y * constants::board_width + x;
            auto current = match.board().pieces()[index];

            auto subregion_x = 0;
            switch (current.type()) {
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
                break;
            }

            if (current.type() != PieceType::None) {
                auto params = DrawParams()
                              .set_position(board_offset.x() + x * square_size, board_offset.y() + y * square_size)
                              .set_scale(sprite_scale, sprite_scale);
                auto region = ImageRegion(
                                  subregion_x,
                                  current.team() == Team::White ? 0 : sprite_size,
                                  sprite_size,
                                  sprite_size
                              );

                draw_texture(ctx, pieces, region, params);
            }
        }
    }

    // Animate the most recent move.
    if (move_was_queued) {
        auto temp_draw_x = current_position.x() * ctx.alpha + previous_position.x() * (1 - ctx.alpha);
        auto temp_draw_y = current_position.y() * ctx.alpha + previous_position.y() * (1 - ctx.alpha);

        if (board_flipped) {
            temp_draw_x = constants::board_width - temp_draw_x - 1;
            temp_draw_y = constants::board_height - temp_draw_y - 1;
        }

        temp_draw_x = temp_draw_x * square_size + board_offset.x();
        temp_draw_y = temp_draw_y * square_size + board_offset.y();

        auto x = queued_move->start().x();
        auto y = queued_move->start().y();

        auto index = y * constants::board_width + x;
        auto current = match.board().pieces()[index];

        auto subregion_x = 0;
        switch (current.type()) {
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
            break;
        }

        if (current.type() != PieceType::None) {
            auto params = DrawParams()
                          .set_position(temp_draw_x, temp_draw_y)
                          .set_scale(sprite_scale, sprite_scale);
            auto region = ImageRegion(
                              subregion_x,
                              current.team() == Team::White ? 0 : sprite_size,
                              sprite_size,
                              sprite_size
                          );

            draw_texture(ctx, pieces, region, params);
        }
    }
}

void Chess::destroy(dam::Context& ctx)
{
    using namespace dam::graphics;

    unload_font(font);
    unload_texture(pieces);
}
