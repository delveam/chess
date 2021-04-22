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
    dam::clear(dam::palette::GREEN);
    dam::draw_texture(64, 64, texture);
    dam::draw_rectangle(5, 5, 126, 123, dam::Color(0xff0000, 0.5));
    dam::draw_text(32, 32, "YEET", font, dam::palette::WHITE);
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
