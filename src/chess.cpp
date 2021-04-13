#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include "chess.hpp"
#include "dam.hpp"

Chess::Chess() {
}

void Chess::initialize() {
	font = al_create_builtin_font();
	texture = al_load_bitmap("content/sprites/pawn.png");
}

void Chess::update() {
}

void Chess::draw() {
	dam::draw_rect(32, 32, 128, 32, al_map_rgb(255, 0, 255));
	dam::draw_text("Hello World", font, 16, 16, 32);
	dam::draw_texture(64, 64, texture);
}

void Chess::destroy() {
	al_destroy_font(font);
	al_destroy_bitmap(texture);
}

Chess::~Chess() {
}
