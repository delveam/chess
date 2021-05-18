#include "input.hpp"

dam::input::KeyboardState dam::input::Keyboard::get_state(dam::Context& ctx)
{
    return dam::input::KeyboardState();
}

bool dam::input::Keyboard::pressing(dam::Context& ctx, dam::input::Key key)
{
    return ctx.keyboard.pressing(key);
}

bool dam::input::Keyboard::pressed(dam::Context& ctx, dam::input::Key key)
{
    return ctx.keyboard.pressed(key);
}

dam::input::MouseState dam::input::Mouse::get_state(dam::Context& ctx)
{
    return dam::input::MouseState();
}

bool dam::input::Mouse::pressing(dam::Context& ctx, dam::input::MouseButton button)
{
    return ctx.mouse.pressing(button);
}

bool dam::input::Mouse::pressed(dam::Context& ctx, dam::input::MouseButton button)
{
    return ctx.mouse.pressed(button);
}

dam::Vector2F dam::input::Mouse::get_position(dam::Context& ctx)
{
    return ctx.mouse.get_position();
}

int dam::input::Mouse::get_scroll_wheel_value(dam::Context& ctx)
{
    return ctx.mouse.get_scroll_wheel_value();
}

int dam::input::Mouse::get_scroll_stride(dam::Context& ctx)
{
    return ctx.mouse.get_scroll_stride();
}
