#ifndef DAM_GRAPHICS_HPP
#define DAM_GRAPHICS_HPP
#include <allegro5/allegro_font.h>
#include "color.hpp"
#include "context.hpp"
#include "draw_params.hpp"

namespace dam {
namespace graphics {
typedef ALLEGRO_BITMAP Texture;
typedef ALLEGRO_FONT Font;

Texture* load_texture(std::string path);
void unload_texture(Texture* texture);
Font* load_font(std::string path, unsigned int size);
void unload_font(Font* font);

void clear(Color color);
void draw_rectangle(Context& ctx, DrawParams params);
void draw_circle(Context& ctx, DrawParams params);
void draw_texture(Context& ctx, Texture* texture, DrawParams params);
void draw_text(Context& ctx, std::string text, Font* font, DrawParams params);
}
}
#endif
