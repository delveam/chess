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
    SmartKeyboard();
    bool pressed(dam::input::Key key) const;
    bool pressing(dam::input::Key key) const;
    void update();
private:
    dam::input::KeyboardState previous;
    dam::input::KeyboardState current;
};
#endif
