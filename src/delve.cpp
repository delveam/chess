#include <string>
#include <allegro5/allegro_font.h>
#include <allegro5/color.h>
#include "delve.hpp"

void dam::draw_text(const char* text, const ALLEGRO_FONT* font, float x, float y, float size) {
    al_draw_text(font, al_map_rgb_f(255, 255, 255), x, y, 0, text);
}
