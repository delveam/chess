#ifndef DAM_VECTOR2F_HPP
#define DAM_VECTOR2F_HPP
namespace dam {
class Vector2F;
}

class dam::Vector2F {
public:
    Vector2F();
    Vector2F(float x, float y);
    float x;
    float y;
};
#endif
