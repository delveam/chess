#ifndef DAM_SMART_KEYBOARD_HPP
#define DAM_SMART_KEYBOARD_HPP
#include "keyboard_state.hpp"

namespace dam {
namespace input {
class SmartKeyboard;
}
}

class dam::input::SmartKeyboard {
public:
    SmartKeyboard() = default;

    bool pressed(dam::input::Key key) const;
    bool pressing(dam::input::Key key) const;
    void update();
private:
    dam::input::KeyboardState m_previous;
    dam::input::KeyboardState m_current;
};
#endif
