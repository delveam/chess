#ifndef DAM_RECTANGLE_HPP
#define DAM_RECTANGLE_HPP
namespace dam {
class RectangleF;
}

class dam::RectangleF {
public:
    RectangleF() = default;
    RectangleF(float x, float y, float width, float height) :
        m_x(x),
        m_y(y),
        m_width(width),
        m_height(height)
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
    float width() const
    {
        return m_width;
    }
    float height() const
    {
        return m_height;
    }
private:
    float m_x { 0 };
    float m_y { 0 };
    float m_width { 0 };
    float m_height { 0 };
};
#endif
