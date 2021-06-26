#include "keyboard_state.hpp"

bool dam::input::KeyboardState::is_key_down(dam::input::Key key) const
{
    return al_key_down(state.get(), static_cast<int>(key));
}

bool dam::input::KeyboardState::is_key_up(dam::input::Key key) const
{
    return !al_key_down(state.get(), static_cast<int>(key));
}
