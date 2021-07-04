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

    if (selected && Mouse::pressed(ctx, MouseButton::Right)) {
        selected = false;
        initial_selection = "";
    }

    if (!selected && Mouse::pressed(ctx, MouseButton::Left)) {
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

            auto coords = Coordinates::create(x, y).value();

            initial_selection = coords.to_string();
        }
    }
    else if (selected && Mouse::pressed(ctx, MouseButton::Left)) {
        auto position = Mouse::get_position(ctx);
        auto x = (int)((position.x() - board_offset.x()) / square_size);
        auto y = (int)((position.y() - board_offset.y()) / square_size);

        if (board_flipped) {
            x = constants::board_width - x - 1;
            y = constants::board_height - y - 1;
        }

        auto target = match.board().get(x, y);

        if (target.has_value()) {
            auto coords = Coordinates::create(x, y).value();

            match.submit_move(initial_selection + coords.to_string());

            selected = false;
            initial_selection = "";
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

    if (selected) {
        auto first_char = initial_selection.substr(0, 1).c_str()[0];
        auto second_char = initial_selection.substr(1, 2);
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
                      .set_tint(Color(0x0000ff, 0.2));

        draw_rectangle(ctx, params);

        auto index = y * constants::board_width + x;
        auto moves = match.moves();
        auto target_move_set = moves.at(index);

        for (const auto& value : target_move_set) {
            // TODO(thismarvin): This is very similar to the code above. How can we get rid of code duplication?
            auto temp_coords = value.substr(2, 4);
            auto temp_x = temp_coords.c_str()[0] - 'a';
            auto temp_y = constants::board_height - std::stoi(temp_coords.substr(1, 2));

            auto temp_draw_x = temp_x;
            auto temp_draw_y = temp_y;

            if (board_flipped) {
                temp_draw_x = constants::board_width - temp_x - 1;
                temp_draw_y = constants::board_height - temp_y - 1;
            }

            temp_draw_x = temp_draw_x * square_size + board_offset.x();
            temp_draw_y = temp_draw_y * square_size + board_offset.y();

            auto target_piece = match.board().get(temp_x, temp_y);

            if (target_piece.has_value() && target_piece->team() == Team::None) {
                auto radius = square_size * 0.4 * 0.5;
                auto temp_params = DrawParams()
                                   .set_position(temp_draw_x + square_size * 0.5 - radius, temp_draw_y + square_size * 0.5 - radius)
                                   .set_scale(radius * 2, radius * 2)
                                   .set_tint(Color(0x0000ff, 0.2));

                draw_circle(ctx, temp_params);
            }
            else {
                auto temp_params = DrawParams()
                                   .set_position(temp_draw_x, temp_draw_y)
                                   .set_scale(square_size, square_size)
                                   .set_tint(Color(0xff0000, 0.2));

                draw_rectangle_but_not_filled(ctx, temp_params, square_size * 0.25);
            }
        }
    }

    al_set_target_backbuffer(ctx.display);
    // Draw pieces.
    for (int y = 0; y < constants::board_height; ++y) {
        for (int x = 0; x < constants::board_width; ++x) {
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
}

void Chess::destroy(dam::Context& ctx)
{
    using namespace dam::graphics;

    unload_font(font);
    unload_texture(pieces);
}
