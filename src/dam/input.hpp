#ifndef DAM_INPUT_HPP
#define DAM_INPUT_HPP
#include "context.hpp"
#include "keyboard_state.hpp"
#include "mouse_state.hpp"

namespace dam {
namespace input {
namespace Keyboard {
KeyboardState get_state(Context& ctx);
bool pressing(Context& ctx, Key key);
bool pressed(Context& ctx, Key key);
}
namespace Mouse {
MouseState get_state(Context& ctx);
bool pressing(Context& ctx, MouseButton button);
bool pressed(Context& ctx, MouseButton button);
Vector2F get_position(Context& ctx);
int get_scroll_wheel_value(Context& ctx);
int get_scroll_stride(Context& ctx);
}
}
}
#endif
