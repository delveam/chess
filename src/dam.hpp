#ifndef DAM_HPP
#define DAM_HPP
#include <string>
#include <allegro5/allegro_font.h>
#include <allegro5/color.h>

namespace dam {
    void draw_text(const char* text, const ALLEGRO_FONT* font, float x, float y, float size);
}
#endif
