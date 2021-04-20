#ifndef DAM_HPP
#define DAM_HPP
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <iostream>
#include <string>
#include "color.hpp"
#include "keyboard_state.hpp"

typedef ALLEGRO_BITMAP Bitmap;
typedef ALLEGRO_FONT Font;

namespace dam {
namespace Keyboard {
KeyboardState get_state();
}

const float PI = ALLEGRO_PI;
const float TAU = ALLEGRO_PI * 2;

Bitmap* load_texture(std::string path);
Font* load_font(std::string path, unsigned int size);
void unload_texture(Bitmap* texture);
void unload_font(Font* font);

void clear(dam::Color color);

void draw_rectangle(float x, float y, float width, float height, dam::Color color);
void draw_circle(float x, float y, float radius, dam::Color color);
void draw_line(float x1, float y1, float x2, float y2, float thickness, dam::Color color);
void draw_texture(float x, float y, Bitmap* texture);
void draw_text(float x, float y, std::string text, Font* font, dam::Color color);
}
#endif
