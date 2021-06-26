#ifndef DAM_IMAGE_REGION_HPP
#define DAM_IMAGE_REGION_HPP
namespace dam {
namespace graphics {
class ImageRegion;
}
}

class dam::graphics::ImageRegion {
public:
    ImageRegion();
    ImageRegion(unsigned int x, unsigned int y, unsigned int width, unsigned int height) :
        m_x(x),
        m_y(y),
        m_width(width),
        m_height(height)
    {
    }

    unsigned int x() const
    {
        return m_x;
    }
    unsigned int y() const
    {
        return m_y;
    }
    unsigned int width() const
    {
        return m_width;
    }
    unsigned int height() const
    {
        return m_height;
    }
private:
    unsigned int m_x { 0 };
    unsigned int m_y { 0 };
    unsigned int m_width { 0 };
    unsigned int m_height { 0 };
};
#endif
