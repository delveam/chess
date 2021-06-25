#include <allegro5/allegro_primitives.h>
#include "graphics.hpp"

ALLEGRO_COLOR convert_dam_color(dam::graphics::Color color)
{
    return al_map_rgba(color.r(), color.g(), color.b(), (int)(color.a() * 255));
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
    auto width = params.scale.x;
    auto height = params.scale.y;
    auto tint = convert_dam_color(params.tint);

    al_draw_filled_rectangle(x, y, x + width, y + height, tint);
}

void dam::graphics::draw_circle(Context& ctx, DrawParams params)
{
    auto x = params.position.x;
    auto y = params.position.y;
    auto width = params.scale.x;
    auto tint = convert_dam_color(params.tint);

    al_draw_filled_circle(x, y, width * 0.5, tint);
}

void dam::graphics::draw_texture(Context& ctx, Texture* texture, ImageRegion region, DrawParams params)
{
    auto subregion_x = region.x;
    auto subregion_y = region.y;
    auto subregion_width = region.width;
    auto subregion_height = region.height;
    auto tint = convert_dam_color(params.tint);
    auto center_x = params.center.x;
    auto center_y = params.center.y;
    auto x = params.position.x;
    auto y = params.position.y;
    auto scale_x = params.scale.x;
    auto scale_y = params.scale.y;
    auto rotation = params.rotation;

    al_draw_tinted_scaled_rotated_bitmap_region(
        texture,
        subregion_x, subregion_y, subregion_width, subregion_height,
        tint,
        center_x, center_y,
        x, y,
        scale_x, scale_y,
        rotation,
        0
    );
}

void dam::graphics::draw_text(Context& ctx, std::string text, Font* font, DrawParams params)
{
    auto x = params.position.x;
    auto y = params.position.y;
    auto tint = convert_dam_color(params.tint);

    al_draw_text(font, tint, x, y, 0, text.c_str());
}
