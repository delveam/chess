#include "color.hpp"

dam::Color::Color()
{
    r = 0;
    g = 0;
    b = 0;
    a = 1;
}

dam::Color::Color(unsigned int hexadecimal, float alpha)
{
    r = (hexadecimal & 0xff0000) >> 16;
    g = (hexadecimal & 0xff00) >> 8;
    b = hexadecimal & 0xff;
    a = 1;

    multiply(alpha);
}

void dam::Color::multiply(float value)
{
    r = r * value;
    g = g * value;
    b = b * value;
    a = a * value;
}
