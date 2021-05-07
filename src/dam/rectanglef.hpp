#ifndef DAM_RECTANGLE_HPP
#define DAM_RECTANGLE_HPP
namespace dam {
class RectangleF;
}

class dam::RectangleF {
public:
    RectangleF();
    RectangleF(float x, float y, float width, float height);
    float x;
    float y;
    float width;
    float height;
};
#endif
