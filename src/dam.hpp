#ifndef DAM_HPP
#define DAM_HPP
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <iostream>
#include <string>
#include "keyboard_state.hpp"

namespace dam {
namespace Keyboard {
KeyboardState get_state();
}

const float PI = ALLEGRO_PI;
const float TAU = ALLEGRO_PI * 2;

ALLEGRO_BITMAP* load_texture(std::string path);
ALLEGRO_FONT* load_font(std::string path, unsigned int size);
void unload_texture(ALLEGRO_BITMAP* texture);
void unload_font(ALLEGRO_FONT* font);

void draw_rectangle(float x, float y, float width, float height, ALLEGRO_COLOR color);
void draw_texture(float x, float y, ALLEGRO_BITMAP* texture);
void draw_text(float x, float y, std::string text, ALLEGRO_FONT* font, ALLEGRO_COLOR color);
}
#endif
