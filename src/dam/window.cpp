#include "window.hpp"

void dam::window::set_title(Context& ctx, std::string title)
{
    al_set_window_title(ctx.display, title.c_str());
}
unsigned int dam::window::get_width(Context& ctx)
{
    return al_get_display_width(ctx.display);
}

unsigned int dam::window::get_height(Context& ctx)
{
    return al_get_display_height(ctx.display);
}

void dam::window::set_dimension(Context& ctx, unsigned int width, unsigned int height)
{
    al_resize_display(ctx.display, width, height);
}

unsigned int dam::window::get_display_width(Context& ctx)
{
    return ctx.display_width;
}

unsigned int dam::window::get_display_height(Context& ctx)
{
    return ctx.display_height;
}
