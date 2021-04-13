#ifndef DAM_HPP
#define DAM_HPP
#include <string>
#include <allegro5/allegro_font.h>
#include <allegro5/color.h>

namespace dam {
	template <typename FONT, typename COLOR>
	void draw_text(const char* text, FONT* font, float x, float y, COLOR color);
	template <typename COLOR>
	void draw_rect(float x, float y, float width, float height, COLOR color);
	template <typename COLOR>
	void draw_quad(float x1, float y1, float x2, float y2, COLOR color);
	template <typename TEXTURE>
	void draw_texture(float x, float y, TEXTURE texture);
}
#include "dam_allegro.hpp"
#endif
