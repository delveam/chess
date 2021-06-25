#include "color.hpp"

dam::graphics::Color::Color(unsigned int hexadecimal, float alpha)
{
    m_r = (hexadecimal & 0xff0000) >> 16;
    m_g = (hexadecimal & 0xff00) >> 8;
    m_b = hexadecimal & 0xff;
    m_a = 1;

    multiply(alpha);
}

void dam::graphics::Color::multiply(float value)
{
    m_r *= value;
    m_g *= value;
    m_b *= value;
    m_a *= value;
}
