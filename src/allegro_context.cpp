#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include "allegro_context.hpp"

std::any AllegroContext::load_texture_impl(std::string path) {
	return std::any(al_load_bitmap(path.c_str()));
}

std::any AllegroContext::load_font_impl(std::string path) {
	return std::any(al_load_bitmap_font(path.c_str()));
}

void AllegroContext::draw_rectangle_impl(float x, float y, float width, float height, std::any color) {
	ALLEGRO_COLOR _color = std::any_cast<ALLEGRO_COLOR>(color);
	al_draw_filled_rectangle(x, y, x + width, y + height, _color);
}

void AllegroContext::draw_texture_impl(float x, float y, std::any texture) {
	ALLEGRO_BITMAP* _texture = std::any_cast<ALLEGRO_BITMAP*>(texture);
	al_draw_bitmap(_texture, x, y, 0);
}

void AllegroContext::draw_text_impl(float x, float y, std::string text, std::any font, std::any color) {
	ALLEGRO_FONT* _font = std::any_cast<ALLEGRO_FONT*>(font);
	ALLEGRO_COLOR _color = std::any_cast<ALLEGRO_COLOR>(color);
	al_draw_text(_font, _color, x, y, 0, text.c_str());
}

void AllegroContext::unload_texture_impl(std::any texture) {
	ALLEGRO_BITMAP* _texture = std::any_cast<ALLEGRO_BITMAP*>(texture);
	al_destroy_bitmap(_texture);
}

void AllegroContext::unload_font_impl(std::any font) {
	ALLEGRO_FONT* _font = std::any_cast<ALLEGRO_FONT*>(font);
	al_destroy_font(_font);
}

std::any AllegroContext::get_keyboard_state_impl() {
	ALLEGRO_KEYBOARD_STATE* state;
	al_get_keyboard_state(state);
	return std::any(state);
}
