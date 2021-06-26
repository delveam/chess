#ifndef DAM_VECTOR2F_HPP
#define DAM_VECTOR2F_HPP
namespace dam {
class Vector2F;
}

class dam::Vector2F {
public:
    Vector2F() = default;
    Vector2F(float x, float y) :
        m_x(x),
        m_y(y)
    {
    }

    float x() const
    {
        return m_x;
    }
    float y() const
    {
        return m_y;
    }
private:
    float m_x { 0 };
    float m_y { 0 };
};
#endif
