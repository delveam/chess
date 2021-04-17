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
    // previous.state.swap(current.state);

    // auto temp = std::make_unique<ALLEGRO_KEYBOARD_STATE>();
    // al_get_keyboard_state(temp.get());

    // current.state.swap(temp);
}
