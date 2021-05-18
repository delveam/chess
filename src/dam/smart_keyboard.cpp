#include "smart_keyboard.hpp"

dam::input::SmartKeyboard::SmartKeyboard() {}

bool dam::input::SmartKeyboard::pressed(dam::input::Key key)
{
    return (current.is_key_down(key) && !previous.is_key_down(key));
}

bool dam::input::SmartKeyboard::pressing(dam::input::Key key)
{
    return current.is_key_down(key);
}

void dam::input::SmartKeyboard::update()
{
    auto temp = dam::input::KeyboardState();
    previous.state.swap(current.state);
    current.state.swap(temp.state);
}
