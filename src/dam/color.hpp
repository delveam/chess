#ifndef DAM_COLOR_HPP
#define DAM_COLOR_HPP
namespace dam {
namespace graphics {
class Color;
}
}

class dam::graphics::Color {
public:
    Color(): m_r(0), m_g(0), m_b(0), m_a(1) {}
    Color(unsigned int hexadecimal, float alpha=1);

    unsigned int r() const
    {
        return m_r;
    }
    unsigned int g() const
    {
        return m_g;
    }
    unsigned int b() const
    {
        return m_b;
    }
    float a() const
    {
        return m_a;
    }

    void multiply(float value);
private:
    unsigned int m_r;
    unsigned int m_g;
    unsigned int m_b;
    float m_a;
};
#endif
