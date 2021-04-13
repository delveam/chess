#ifndef DAM_ALLEGRO_HPP
#define DAM_ALLEGRO_HPP
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

namespace dam {
	template <typename T>
	void draw_text(const char* text, T* font, float x, float y, float size) {
		al_draw_text(font, al_map_rgb_f(255, 255, 255), x, y, 0, text);
	}
	template <typename T>
	void draw_rect(float x, float y, float width, float height, T color) {
		al_draw_filled_rectangle(x, y, x + width, y + height, color);
	}
	template <typename T>
	void draw_quad(float x1, float y1, float x2, float y2, T color) {
		al_draw_filled_rectangle(x1, y1, x2, y2, color);
	}
	template <typename T>
	void draw_texture(float x, float y, T texture) {
		al_draw_bitmap(texture, x, y, 0);
	}
}
#endif
