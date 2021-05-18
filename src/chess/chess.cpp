#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <iostream>
#include <memory>
#include "chess.hpp"
#include "../dam/graphics.hpp"
#include "../dam/input.hpp"
#include "../dam/palette.hpp"
#include "../dam/window.hpp"

Chess::Chess()
{
}

void Chess::initialize(dam::Context& ctx)
{
    using namespace dam::graphics;

    font = load_font("./content/fonts/FiraCode-SemiBold.ttf", 16);

    pieces = load_texture("./content/sprites/pieces.png");

    // TODO: Handle condition when the board is too big (check the width not height in this case).
    square_size = dam::window::get_height(ctx) / BOARD_HEIGHT;
    board_offset = dam::Vector2F((dam::window::get_width(ctx) - (square_size * BOARD_WIDTH)) / 2, 0);

    board = Board::load_from_fen(STARTING_FEN);
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

    // TODO: We need to handle when the board is flipped!
    if (!selected && Mouse::pressed(ctx, MouseButton::Left)) {
        auto position = Mouse::get_position(ctx);
        auto x = (int)((position.x - board_offset.x) / square_size);
        auto y = (int)((position.y - board_offset.y) / square_size);
        if (board.get(x, y).type != PieceType::None) {
            selected = true;
            initial_selection = "";
            initial_selection.push_back('a' + x);
            initial_selection.push_back('0' + BOARD_HEIGHT - y);
        }
    }
    else if (selected && Mouse::pressed(ctx, MouseButton::Left)) {
        selected = false;
        auto position = Mouse::get_position(ctx);
        auto x = (int)((position.x - board_offset.x) / square_size);
        auto y = (int)((position.y - board_offset.y) / square_size);
        std::string second = "";
        second.push_back('a' + x);
        second.push_back('0' + BOARD_HEIGHT - y);
        board.move_uci(initial_selection + second);
    }
}

void Chess::event(dam::Context& ctx, dam::EventType event)
{
    switch (event) {
    case dam::EventType::WindowResize:
        square_size = dam::window::get_height(ctx) / BOARD_HEIGHT;
        board_offset = dam::Vector2F((dam::window::get_width(ctx) - (square_size * BOARD_WIDTH)) / 2, 0);
        break;
    default:
        break;
    }
}

void Chess::draw(dam::Context& ctx)
{
    using namespace dam::graphics;

    clear(palette::BLACK);

    auto light_color = Color(0xeeeed2);
    auto dark_color = Color(0x769656);

    // Draw board.
    for (int y = 0; y < BOARD_HEIGHT; ++y) {
        for (int x = 0; x < BOARD_WIDTH; ++x) {
            auto color = (x + y) % 2 == 0 ? light_color : dark_color;

            auto params = DrawParams();
            params.set_position(board_offset.x + x * square_size, y * square_size);
            params.set_scale(square_size, square_size);
            params.set_tint(color);
            draw_rectangle(ctx, params);
        }
    }

    // Draw algebraic notations.
    for (int x = 0; x < BOARD_WIDTH; ++x) {
        auto color = x % 2 == 0 ? light_color : dark_color;
        auto temp = !board_flipped ? 'a' + x : 'a' + (BOARD_WIDTH - 1 - x);
        std::string text = "";
        text.push_back(temp);

        auto params = DrawParams();
        params.set_position(board_offset.x + x * square_size + square_size * 0.08, (BOARD_HEIGHT - 1) * square_size + square_size * 0.75);
        params.set_tint(color);
        draw_text(ctx, text, font, params);
    }
    for (int y = 0; y < BOARD_HEIGHT; ++y) {
        auto color = y % 2 == 0 ? light_color : dark_color;
        auto temp = !board_flipped ? '1' + (BOARD_HEIGHT - 1 - y) : '1' + y;
        std::string text = "";
        text.push_back(temp);

        auto params = DrawParams();
        params.set_position(board_offset.x + (BOARD_WIDTH - 1) * square_size + square_size * 0.75, y * square_size + square_size * 0.08);
        params.set_tint(color);
        draw_text(ctx, text, font, params);
    }

    if (selected) {
        auto params = DrawParams();
        auto first_char = initial_selection.substr(0, 1)[0];
        auto second_char = initial_selection.substr(1, 2);
        auto x = first_char - 'a';
        auto y = BOARD_HEIGHT - std::stoi(second_char);
        x *= square_size;
        x += board_offset.x;
        y *= square_size;
        y += board_offset.y;
        params.set_position(x, y);
        params.set_scale(square_size, square_size);
        params.set_tint(Color(0x0000ff, 0.2));

        draw_rectangle(ctx, params);
    }

    al_set_target_backbuffer(ctx.display);
    // Draw pieces.
    auto scale = square_size / 16.0;
    for (int y = 0; y < BOARD_HEIGHT; ++y) {
        for (int x = 0; x < BOARD_WIDTH; ++x) {
            auto index = board_flipped ? ((BOARD_HEIGHT - 1) - y) * BOARD_WIDTH + ((BOARD_WIDTH - 1) - x) : y * BOARD_WIDTH + x;
            auto current = board.pieces[index];

            auto subregion_x = 0;
            switch (current.type) {
            case PieceType::Pawn:
                break;
            case PieceType::Bishop:
                subregion_x = 16;
                break;
            case PieceType::Knight:
                subregion_x = 32;
                break;
            case PieceType::Rook:
                subregion_x = 48;
                break;
            case PieceType::Queen:
                subregion_x = 64;
                break;
            case PieceType::King:
                subregion_x = 80;
                break;
            case PieceType::None:
                break;
            }

            if (current.type != PieceType::None) {
                auto params = DrawParams();
                params.position.x = board_offset.x + x * square_size;
                params.position.y = y * square_size;
                params.scale.x = scale;
                params.scale.y = scale;
                auto region = ImageRegion();
                region.x = subregion_x;
                region.y = current.team == Team::White ? 0 : 16;
                region.width = 16;
                region.height = 16;

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

Chess::~Chess()
{
}
