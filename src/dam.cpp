#include "dam.hpp"

dam::KeyboardState dam::Keyboard::get_state()
{
    return dam::KeyboardState();
}

ALLEGRO_BITMAP* dam::load_texture(std::string path)
{
    return al_load_bitmap(path.c_str());
}

ALLEGRO_FONT* dam::load_font(std::string path, unsigned int size)
{
    return al_load_font(path.c_str(), size, 0);
}

void dam::unload_texture(ALLEGRO_BITMAP* texture)
{
    al_destroy_bitmap(texture);
}

void dam::unload_font(ALLEGRO_FONT* font)
{
    al_destroy_font(font);
}

void dam::draw_rectangle(float x, float y, float width, float height, ALLEGRO_COLOR color)
{
    al_draw_filled_rectangle(x, y, x + width, y + height, color);
}

void dam::draw_texture(float x, float y, ALLEGRO_BITMAP* texture)
{
    al_draw_bitmap(texture, x, y, 0);
}

void dam::draw_text(float x, float y, std::string text, ALLEGRO_FONT* font, ALLEGRO_COLOR color)
{
    al_draw_text(font, color, x, y, 0, text.c_str());
}
