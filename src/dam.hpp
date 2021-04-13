#ifndef DAM_HPP
#define DAM_HPP
#include <string>
#include <allegro5/allegro_font.h>
#include <allegro5/color.h>

namespace dam {
	template <typename T>
	void draw_text(const char* text, T* font, float x, float y, float size);
	template <typename T>
	void draw_rect(float x, float y, float width, float height, T color);
	template <typename T>
	void draw_quad(float x1, float y1, float x2, float y2, T color);
	template <typename T>
	void draw_texture(float x, float y, T texture);
}
#include "dam_allegro.hpp"
#endif
