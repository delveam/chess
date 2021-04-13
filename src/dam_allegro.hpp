#ifndef DAM_ALLEGRO_HPP
#define DAM_ALLEGRO_HPP
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

namespace dam {
	template <typename FONT, typename COLOR>
	void draw_text(const char* text, FONT* font, float x, float y, COLOR color) {
		al_draw_text(font, color, x, y, 0, text);
	}
	template <typename COLOR>
	void draw_rect(float x, float y, float width, float height, COLOR color) {
		al_draw_filled_rectangle(x, y, x + width, y + height, color);
	}
	template <typename COLOR>
	void draw_quad(float x1, float y1, float x2, float y2, COLOR color) {
		al_draw_filled_rectangle(x1, y1, x2, y2, color);
	}
	template <typename TEXTURE>
	void draw_texture(float x, float y, TEXTURE texture) {
		al_draw_bitmap(texture, x, y, 0);
	}
}
#endif
