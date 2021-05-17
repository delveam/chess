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

    font = al_create_builtin_font();

    pieces = load_texture("./content/sprites/pieces.png");

    board = Board::load_from_fen(STARTING_FEN);
    board = Board::load_from_fen("4r1k1/1p3q1p/p1pQ4/2P1R1p1/5n2/2B5/PP5P/6K1 b - - 0 1");
    board_flipped = false;
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
}

void Chess::draw(dam::Context& ctx)
{
    using namespace dam::graphics;

    clear(palette::BLACK);

    auto size = dam::window::get_height(ctx) / BOARD_HEIGHT;
    auto offset = (dam::window::get_width(ctx) - (size * BOARD_WIDTH)) / 2;
    auto light_color = Color(0xeeeed2);
    auto dark_color = Color(0x769656);

    // Draw board.
    for (int y = 0; y < BOARD_HEIGHT; ++y) {
        for (int x = 0; x < BOARD_WIDTH; ++x) {
            auto color = (x + y) % 2 == 0 ? light_color : dark_color;

            auto params = DrawParams();
            params.set_positon(offset + x * size, y * size);
            params.set_scale(size, size);
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
        params.set_positon(offset + x * size + size * 0.08, (BOARD_HEIGHT - 1) * size + size * 0.75);
        params.set_tint(color);
        draw_text(ctx, text, font, params);
    }
    for (int y = 0; y < BOARD_HEIGHT; ++y) {
        auto color = y % 2 == 0 ? light_color : dark_color;
        auto temp = !board_flipped ? '1' + (BOARD_HEIGHT - 1 - y) : '1' + y;
        std::string text = "";
        text.push_back(temp);

        auto params = DrawParams();
        params.set_positon(offset + (BOARD_WIDTH - 1) * size + size * 0.75, y * size + size * 0.08);
        params.set_tint(color);
        draw_text(ctx, text, font, params);
    }

    al_set_target_backbuffer(ctx.display);
    // Draw pieces.
    auto scale = size / 16.0;
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
                params.position.x = offset + x * size;
                params.position.y = y * size;
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
