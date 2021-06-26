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
    SmartMouse() = default;

    bool pressed(dam::input::MouseButton button) const;
    bool pressing(dam::input::MouseButton button) const;
    Vector2F get_position() const;
    int get_scroll_wheel_value() const;
    int get_scroll_stride() const;
    void update();
private:
    dam::input::MouseState m_previous;
    dam::input::MouseState m_current;
};
#endif
