#include "keyboard_state.hpp"

dam::input::KeyboardState::KeyboardState()
{
    state = std::make_unique<ALLEGRO_KEYBOARD_STATE>();
    al_get_keyboard_state(state.get());
}

bool dam::input::KeyboardState::is_key_down(dam::input::Key key)
{
    return al_key_down(state.get(), key);
}

bool dam::input::KeyboardState::is_key_up(dam::input::Key key)
{
    return !al_key_down(state.get(), key);
}
