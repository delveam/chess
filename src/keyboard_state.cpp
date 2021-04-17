#include "keyboard_state.hpp"

dam::KeyboardState::KeyboardState()
{
    state = std::make_unique<ALLEGRO_KEYBOARD_STATE>();
    al_get_keyboard_state(state.get());
}

bool dam::KeyboardState::is_key_down(int key)
{
    return al_key_down(state.get(), key);
}

bool dam::KeyboardState::is_key_up(int key)
{
    return !al_key_down(state.get(), key);
}
