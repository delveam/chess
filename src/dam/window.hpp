#ifndef WINDOW_HPP
#define WINDOW_HPP
#include "context.hpp"

namespace dam {
namespace window {
void set_title(Context& ctx, std::string title);
unsigned int get_width(Context& ctx);
unsigned int get_height(Context& ctx);
void set_dimension(Context& ctx, unsigned int width, unsigned int height);

unsigned int get_display_width(Context& ctx);
unsigned int get_display_height(Context& ctx);
}
}
#endif
