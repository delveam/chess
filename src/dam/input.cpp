#include "input.hpp"

dam::input::KeyboardState dam::input::Keyboard::get_state(dam::Context ctx)
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
