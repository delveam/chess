#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <iostream>
#include <memory>
#include "chess.hpp"
#include "../dam/palette.hpp"

Chess::Chess()
{
}

void Chess::initialize()
{
    font = al_create_builtin_font();

    pawn_black = dam::load_texture("./content/sprites/pawn_black.png");
    bishop_black = dam::load_texture("./content/sprites/bishop_black.png");
    knight_black = dam::load_texture("./content/sprites/knight_black.png");
    rook_black = dam::load_texture("./content/sprites/rook_black.png");
    queen_black = dam::load_texture("./content/sprites/queen_black.png");
    king_black = dam::load_texture("./content/sprites/king_black.png");
    pawn_white = dam::load_texture("./content/sprites/pawn_white.png");
    bishop_white = dam::load_texture("./content/sprites/bishop_white.png");
    knight_white = dam::load_texture("./content/sprites/knight_white.png");
    rook_white = dam::load_texture("./content/sprites/rook_white.png");
    queen_white = dam::load_texture("./content/sprites/queen_white.png");
    king_white = dam::load_texture("./content/sprites/king_white.png");

    keyboard = std::make_unique<dam::SmartKeyboard>();

    board = Board::load_from_fen(STARTING_FEN);
    board_flipped = false;
}

void Chess::update()
{
    keyboard->update();

    if (keyboard->pressed(ALLEGRO_KEY_ESCAPE)) {
        loop = false;
    }

    if (keyboard->pressed(ALLEGRO_KEY_F)) {
        board_flipped = !board_flipped;
    }
}

void Chess::draw()
{
    dam::clear(dam::palette::BLACK);

    auto size = 60;
    auto offset = (640 - BOARD_WIDTH * size) / 2;
    for (int y = 0; y < BOARD_HEIGHT; ++y) {
        for (int x = 0; x < BOARD_WIDTH; ++x) {
            auto color = dam::Color(0x769656);
            if ((x + y) % 2 == 0) {
                color = dam::Color(0xeeeed2);
            }

            dam::draw_rectangle(offset + x * size, y * size, size, size, color);
        }
    }

    for (int y = 0; y < BOARD_HEIGHT; ++y) {
        for (int x = 0; x < BOARD_WIDTH; ++x) {
            auto index = board_flipped ? ((BOARD_HEIGHT - 1) - y) * BOARD_WIDTH + ((BOARD_WIDTH - 1) - x) : y * BOARD_WIDTH + x;
            auto current = board.pieces[index];

            ALLEGRO_BITMAP* texture = NULL;
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
                dam::draw_texture(offset + x * size, y * size, texture);
            }
        }
    }
}

void Chess::destroy()
{
    dam::unload_font(font);

    dam::unload_texture(pawn_black);
    dam::unload_texture(bishop_black);
    dam::unload_texture(knight_black);
    dam::unload_texture(rook_black);
    dam::unload_texture(queen_black);
    dam::unload_texture(king_black);
    dam::unload_texture(pawn_white);
    dam::unload_texture(bishop_white);
    dam::unload_texture(knight_white);
    dam::unload_texture(rook_white);
    dam::unload_texture(queen_white);
    dam::unload_texture(king_white);
}

Chess::~Chess()
{
}
