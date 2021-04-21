#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <iostream>
#include <memory>
#include "chess.hpp"
#include "dam/palette.hpp"

Chess::Chess()
{
}

void Chess::initialize()
{
    font = al_create_builtin_font();
    texture = dam::load_texture("content/sprites/pawn.png");
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
    dam::unload_texture(texture);
}

Chess::~Chess()
{
}
