#include "mouse_state.hpp"

bool dam::input::MouseState::is_button_down(dam::input::MouseButton button) const
{
    return state.get()->buttons & static_cast<int>(button);
}

bool dam::input::MouseState::is_button_up(dam::input::MouseButton button) const
{
    return !(state.get()->buttons & static_cast<int>(button));
}

dam::Vector2F dam::input::MouseState::get_position() const
{
    return Vector2F(state.get()->x, state.get()->y);
}

int dam::input::MouseState::get_scroll_wheel_value() const
{
    return state.get()->z;
}
