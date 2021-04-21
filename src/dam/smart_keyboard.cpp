#include "smart_keyboard.hpp"

dam::SmartKeyboard::SmartKeyboard() {}

bool dam::SmartKeyboard::pressed(int key)
{
    return (current.is_key_down(key) && !previous.is_key_down(key));
}

bool dam::SmartKeyboard::pressing(int key)
{
    return current.is_key_down(key);
}

void dam::SmartKeyboard::update()
{
    auto temp = dam::KeyboardState();
    previous.state.swap(current.state);
    current.state.swap(temp.state);
}
