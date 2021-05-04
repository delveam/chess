#ifndef COLOR_HPP
#define COLOR_HPP
namespace dam {
namespace graphics {
class Color;
}
}

class dam::graphics::Color {
public:
    Color();
    Color(unsigned int hexadecimal, float alpha=1);
    unsigned int r;
    unsigned int g;
    unsigned int b;
    float a;
    void multiply(float value);
};
#endif
