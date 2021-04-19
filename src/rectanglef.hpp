#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP
namespace dam {
class RectangleF;
}

class dam::RectangleF {
public:
    RectangleF(float x, float y, float width, float height);
    float x;
    float y;
    float width;
    float height;
};
#endif
