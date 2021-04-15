#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include "chess.hpp"

Chess::Chess() {
}

void Chess::initialize(DamContext& context) {
	font = al_create_builtin_font();
	texture = context.load_texture<ALLEGRO_BITMAP*>("content/sprites/pawn.png");
}

void Chess::update(DamContext& context) {
}

void Chess::draw(DamContext& context) {
	context.draw_rectangle(128, 128, 256, 300, al_map_rgb(255, 0, 0));
	context.draw_text(32, 32, "yeet", font, al_map_rgb(255, 255, 255));
	context.draw_texture(64, 64, texture);
}

void Chess::destroy(DamContext& context) {
	al_destroy_font(font);
	context.unload_texture(texture);
}

Chess::~Chess() {
}
