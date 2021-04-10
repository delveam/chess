#include <iostream>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include "game.hpp"

game::Chess::Chess() {
    
}

void game::Chess::initialize() {
    font = al_create_builtin_font();
}

void game::Chess::update() {
}

void game::Chess::draw() {
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_text(font, al_map_rgb(255, 255, 255), 0, 0, 0, "Hello world!");
    al_flip_display();
}

game::Chess::~Chess() {
   al_destroy_font(font);
}
