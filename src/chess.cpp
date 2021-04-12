#include <allegro5/allegro_font.h>
#include "chess.hpp"
#include "dam.hpp"

Chess::Chess() {
}

void Chess::initialize() {
	font = al_create_builtin_font();
}

void Chess::update() {
}

void Chess::draw() {
	dam::draw_text("Hello World", font, 16, 16, 32);
}

void Chess::destroy() {
	 al_destroy_font(font);
}

Chess::~Chess() {
}
