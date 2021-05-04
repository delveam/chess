#include <allegro5/allegro_primitives.h>
#include "graphics.hpp"

ALLEGRO_COLOR convert_dam_color(dam::graphics::Color color)
{
    return al_map_rgba(color.r, color.g, color.b, color.a * 255);
}

dam::graphics::Texture* dam::graphics::load_texture(std::string path)
{
    return al_load_bitmap(path.c_str());
}

dam::graphics::Font* dam::graphics::load_font(std::string path, unsigned int size)
{
    return al_load_font(path.c_str(), size, 0);
}

void dam::graphics::unload_texture(Texture* texture)
{
    al_destroy_bitmap(texture);
}

void dam::graphics::unload_font(Font* font)
{
    al_destroy_font(font);
}

void dam::graphics::clear(Color color)
{
    auto temp = convert_dam_color(color);
    al_clear_to_color(temp);
}

void dam::graphics::draw_rectangle(Context& ctx, DrawParams params)
{
    auto x = params.position.x;
    auto y = params.position.y;
    auto width = std::get<0>(params.dimension);
    auto height = std::get<1>(params.dimension);
    auto color = convert_dam_color(params.color);

    al_draw_filled_rectangle(x, y, x + width, y + height, color);
}

void dam::graphics::draw_circle(Context& ctx, DrawParams params)
{
    auto x = params.position.x;
    auto y = params.position.y;
    auto width = std::get<0>(params.dimension);
    auto color = convert_dam_color(params.color);

    al_draw_filled_circle(x, y, width * 0.5, color);
}

void dam::graphics::draw_texture(Context& ctx, Texture* texture, DrawParams params)
{
    auto x = params.position.x;
    auto y = params.position.y;

    al_draw_bitmap(texture, x, y, 0);
}

void dam::graphics::draw_text(Context& ctx, std::string text, Font* font, DrawParams params)
{
    auto x = params.position.x;
    auto y = params.position.y;
    auto color = convert_dam_color(params.color);

    al_draw_text(font, color, x, y, 0, text.c_str());
}
