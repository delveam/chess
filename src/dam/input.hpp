#ifndef INPUT_HPP
#define INPUT_HPP
#include "context.hpp"
#include "key.hpp"
#include "keyboard_state.hpp"

namespace dam {
namespace input {
namespace Keyboard {
KeyboardState get_state(Context ctx);
bool pressing(Context& ctx, Key key);
bool pressed(Context& ctx, Key key);
}
}
}

#endif
