#ifndef DAM_COLOR_HPP
#define DAM_COLOR_HPP
namespace dam {
namespace graphics {
class Color;
}
}

class dam::graphics::Color {
public:
    Color() = default;
    // FIXME(thismarvin): Should Color just have a single `uint hex` property so we can use a member initialization list here?
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
    unsigned int m_r { 0 };
    unsigned int m_g { 0 };
    unsigned int m_b { 0 };
    float m_a { 1 };
};
#endif
