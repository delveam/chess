#ifndef SMART_KEYBOARD_HPP
#define SMART_KEYBOARD_HPP
#include "keyboard_state.hpp"

namespace dam {
class SmartKeyboard;
}

class dam::SmartKeyboard {
public:
    SmartKeyboard();
    bool pressed(int key);
    bool pressing(int key);
    void update();
private:
    dam::KeyboardState previous;
    dam::KeyboardState current;
};
#endif
