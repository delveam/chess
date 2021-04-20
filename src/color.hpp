#ifndef COLOR_HPP
#define COLOR_HPP
namespace dam {
class Color;
}

class dam::Color {
public:
    Color();
    Color(unsigned int hexadecimal, float alpha=1);
    float r;
    float g;
    float b;
    float a;
    void multiply(float value);
};
#endif
