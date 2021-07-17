#include "vector2f.hpp"

// TODO(thismarvin): Everything...

dam::Vector2F dam::Vector2F::lerp_precise(Vector2F a, Vector2F b, float step)
{
    auto x = (1 - step) * a.x() + step * b.x();
    auto y = (1 - step) * a.y() + step * b.y();

    return dam::Vector2F(x, y);
}
