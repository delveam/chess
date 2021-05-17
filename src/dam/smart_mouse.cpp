#include "smart_mouse.hpp"

dam::input::SmartMouse::SmartMouse() {}

bool dam::input::SmartMouse::pressed(dam::input::MouseButton button)
{
    return (current.is_button_down(button) && !previous.is_button_down(button));
}

bool dam::input::SmartMouse::pressing(dam::input::MouseButton button)
{
    return current.is_button_down(button);
}

dam::Vector2F dam::input::SmartMouse::get_position()
{
    return current.get_position();
}

int dam::input::SmartMouse::get_scroll_wheel_value()
{
    return current.get_scroll_wheel_value();
}

int dam::input::SmartMouse::get_scroll_stride()
{
    return current.get_scroll_wheel_value() - previous.get_scroll_wheel_value();
}

void dam::input::SmartMouse::update()
{
    auto temp = dam::input::MouseState();
    previous.state.swap(current.state);
    current.state.swap(temp.state);
}
