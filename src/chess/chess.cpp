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

    board = Board::load_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w QKqk - 0 1");
}

void Chess::update()
{
    keyboard->update();

    if (keyboard->pressed(ALLEGRO_KEY_ESCAPE)) {
        loop = false;
    }
}

void Chess::draw()
{
    dam::clear(dam::palette::BLACK);

    auto board_size = 8;
    auto size = 60;
    auto offset = (640 - board_size * size) / 2;
    for (int y = 0; y < board_size; ++y) {
        for (int x = 0; x < board_size; ++x) {
            auto color = dam::Color(0x769656);
            if ((x + y) % 2 == 0) {
                color = dam::Color(0xeeeed2);
            }

            dam::draw_rectangle(offset + x * size, y * size, size, size, color);
        }
    }

    for (int y = 0; y < board_size; ++y) {
        for (int x = 0; x < board_size; ++x) {
            auto index = y * board_size + x;
            auto current = board.pieces[index];

            ALLEGRO_BITMAP* texture = NULL;
            switch (current.type) {
            case utils::Pawn:
                texture = current.team == utils::Player::White ? pawn_white : pawn_black;
                break;
            case utils::Knight:
                texture = current.team == utils::Player::White ? knight_white : knight_black;
                break;
            case utils::Bishop:
                texture = current.team == utils::Player::White ? bishop_white : bishop_black;
                break;
            case utils::Rook:
                texture = current.team == utils::Player::White ? rook_white : rook_black;
                break;
            case utils::Queen:
                texture = current.team == utils::Player::White ? queen_white : queen_black;
                break;
            case utils::King:
                texture = current.team == utils::Player::White ? king_white : king_black;
                break;
            case utils::None:
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
