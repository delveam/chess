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

    pawn_black = load_texture("./content/sprites/pawn_black.png");
    bishop_black = load_texture("./content/sprites/bishop_black.png");
    knight_black = load_texture("./content/sprites/knight_black.png");
    rook_black = load_texture("./content/sprites/rook_black.png");
    queen_black = load_texture("./content/sprites/queen_black.png");
    king_black = load_texture("./content/sprites/king_black.png");
    pawn_white = load_texture("./content/sprites/pawn_white.png");
    bishop_white = load_texture("./content/sprites/bishop_white.png");
    knight_white = load_texture("./content/sprites/knight_white.png");
    rook_white = load_texture("./content/sprites/rook_white.png");
    queen_white = load_texture("./content/sprites/queen_white.png");
    king_white = load_texture("./content/sprites/king_white.png");

    board = Board::load_from_fen(STARTING_FEN);
    board = Board::load_from_fen("4r1k1/1p3q1p/p1pQ4/2P1R1p1/5n2/2B5/PP5P/6K1 b - - 0 1");
    board_flipped = false;
}

void Chess::update(dam::Context& ctx)
{
    using namespace dam::input;

    if (Keyboard::pressed(ctx, dam::input::Key::Escape)) {
        loop = false;
    }

    if (Keyboard::pressed(ctx, dam::input::Key::F)) {
        board_flipped = !board_flipped;
    }
}

void Chess::draw(dam::Context& ctx)
{
    using namespace dam::graphics;

    clear(palette::BLACK);

    auto size = 60;
    auto offset = (640 - BOARD_WIDTH * size) / 2;

    // Draw board.
    for (int y = 0; y < BOARD_HEIGHT; ++y) {
        for (int x = 0; x < BOARD_WIDTH; ++x) {
            auto color = Color(0x769656);
            if ((x + y) % 2 == 0) {
                color = Color(0xeeeed2);
            }

            auto params = DrawParams();
            params.set_positon(offset + x * size, y * size);
            params.set_dimension(size, size);
            params.set_color(color);
            draw_rectangle(ctx, params);
        }
    }

    // Draw algebraic notations.
    for (int x = 0; x < BOARD_WIDTH; ++x) {
        auto color = x % 2 == 0 ? Color(0xeeeed2) : Color(0x769656);
        auto temp = !board_flipped ? 'a' + x : 'a' + (BOARD_WIDTH - 1 - x);
        std::string text = "";
        text.push_back(temp);

        auto params = DrawParams();
        params.set_positon(offset + x * size + 4, (BOARD_HEIGHT - 1) * size + 48);
        params.set_color(color);
        draw_text(ctx, text, font, params);
    }
    for (int y = 0; y < BOARD_HEIGHT; ++y) {
        auto color = y % 2 == 0 ? Color(0xeeeed2) : Color(0x769656);
        auto temp = !board_flipped ? '1' + (BOARD_HEIGHT - 1 - y) : '1' + y;
        std::string text = "";
        text.push_back(temp);

        auto params = DrawParams();
        params.set_positon(offset + (BOARD_WIDTH - 1) * size + 48, y * size + 4);
        params.set_color(color);
        draw_text(ctx, text, font, params);
    }

    // Draw pieces.
    for (int y = 0; y < BOARD_HEIGHT; ++y) {
        for (int x = 0; x < BOARD_WIDTH; ++x) {
            auto index = board_flipped ? ((BOARD_HEIGHT - 1) - y) * BOARD_WIDTH + ((BOARD_WIDTH - 1) - x) : y * BOARD_WIDTH + x;
            auto current = board.pieces[index];

            Texture* texture = NULL;
            switch (current.type) {
            case Pawn:
                texture = current.team == Team::White ? pawn_white : pawn_black;
                break;
            case Knight:
                texture = current.team == Team::White ? knight_white : knight_black;
                break;
            case Bishop:
                texture = current.team == Team::White ? bishop_white : bishop_black;
                break;
            case Rook:
                texture = current.team == Team::White ? rook_white : rook_black;
                break;
            case Queen:
                texture = current.team == Team::White ? queen_white : queen_black;
                break;
            case King:
                texture = current.team == Team::White ? king_white : king_black;
                break;
            case None:
                break;
            }

            if (texture != NULL) {
                auto params = DrawParams();
                params.position.x = offset + x * size;
                params.position.y = y * size;

                draw_texture(ctx, texture, params);
            }
        }
    }
}

void Chess::destroy(dam::Context& ctx)
{
    using namespace dam::graphics;

    unload_font(font);

    unload_texture(pawn_black);
    unload_texture(bishop_black);
    unload_texture(knight_black);
    unload_texture(rook_black);
    unload_texture(queen_black);
    unload_texture(king_black);
    unload_texture(pawn_white);
    unload_texture(bishop_white);
    unload_texture(knight_white);
    unload_texture(rook_white);
    unload_texture(queen_white);
    unload_texture(king_white);
}

Chess::~Chess()
{
}
