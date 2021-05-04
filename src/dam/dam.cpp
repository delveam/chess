// #include "dam.hpp"

// ALLEGRO_COLOR convert_dam_color(dam::Color color)
// {
//     return al_map_rgba(color.r, color.g, color.b, color.a * 255);
// }

// dam::KeyboardState dam::Keyboard::get_state()
// {
//     return dam::KeyboardState();
// }

// ALLEGRO_BITMAP* dam::load_texture(std::string path)
// {
//     return al_load_bitmap(path.c_str());
// }

// ALLEGRO_FONT* dam::load_font(std::string path, unsigned int size)
// {
//     return al_load_font(path.c_str(), size, 0);
// }

// void dam::unload_texture(ALLEGRO_BITMAP* texture)
// {
//     al_destroy_bitmap(texture);
// }

// void dam::unload_font(ALLEGRO_FONT* font)
// {
//     al_destroy_font(font);
// }

// void dam::clear(dam::Color color)
// {
//     auto temp = convert_dam_color(color);
//     al_clear_to_color(temp);
// }

// void dam::draw_rectangle(float x, float y, float width, float height, dam::Color color)
// {
//     auto temp = convert_dam_color(color);
//     al_draw_filled_rectangle(x, y, x + width, y + height, temp);
// }

// void dam::draw_circle(float x, float y, float radius, dam::Color color)
// {
//     auto temp = convert_dam_color(color);
//     al_draw_filled_circle(x, y, radius, temp);
// }

// void dam::draw_line(float x1, float y1, float x2, float y2, float thickness, dam::Color color)
// {
//     auto temp = convert_dam_color(color);
//     al_draw_line(x1, y1, x2, y2, temp, thickness);
// }

// void dam::draw_texture(float x, float y, ALLEGRO_BITMAP* texture)
// {
//     al_draw_bitmap(texture, x, y, 0);
// }

// void dam::draw_text(float x, float y, std::string text, ALLEGRO_FONT* font, dam::Color color)
// {
//     auto temp = convert_dam_color(color);
//     al_draw_text(font, temp, x, y, 0, text.c_str());
// }
