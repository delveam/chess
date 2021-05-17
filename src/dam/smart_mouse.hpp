#ifndef DAM_SMART_MOUSE_HPP
#define DAM_SMART_MOUSE_HPP
#include "mouse_state.hpp"

namespace dam {
namespace input {
class SmartMouse;
}
}

class dam::input::SmartMouse {
public:
    SmartMouse();
    bool pressed(dam::input::MouseButton button);
    bool pressing(dam::input::MouseButton button);
    Vector2F get_position();
    int get_scroll_wheel_value();
    int get_scroll_stride();
    void update();
private:
    dam::input::MouseState previous;
    dam::input::MouseState current;
};
#endif
