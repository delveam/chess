#include "smart_mouse.hpp"

bool dam::input::SmartMouse::pressed(dam::input::MouseButton button) const
{
    return (m_current.is_button_down(button) && !m_previous.is_button_down(button));
}

bool dam::input::SmartMouse::pressing(dam::input::MouseButton button) const
{
    return m_current.is_button_down(button);
}

dam::Vector2F dam::input::SmartMouse::get_position() const
{
    return m_current.get_position();
}

int dam::input::SmartMouse::get_scroll_wheel_value() const
{
    return m_current.get_scroll_wheel_value();
}

int dam::input::SmartMouse::get_scroll_stride() const
{
    return m_current.get_scroll_wheel_value() - m_previous.get_scroll_wheel_value();
}

void dam::input::SmartMouse::update()
{
    auto temp = dam::input::MouseState();
    m_previous.state.swap(m_current.state);
    m_current.state.swap(temp.state);
}
