#include "smart_keyboard.hpp"

bool dam::input::SmartKeyboard::pressed(dam::input::Key key) const
{
    return (m_current.is_key_down(key) && !m_previous.is_key_down(key));
}

bool dam::input::SmartKeyboard::pressing(dam::input::Key key) const
{
    return m_current.is_key_down(key);
}

void dam::input::SmartKeyboard::update()
{
    auto temp = dam::input::KeyboardState();
    m_previous.state.swap(m_current.state);
    m_current.state.swap(temp.state);
}
