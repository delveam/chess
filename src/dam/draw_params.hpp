#ifndef DAM_DRAW_PARAMS_HPP
#define DAM_DRAW_PARAMS_HPP
#include "color.hpp"
#include "rectanglef.hpp"
#include "vector2f.hpp"

namespace dam {
namespace graphics {
class DrawParams;
}
}

class dam::graphics::DrawParams {
public:
    DrawParams();
    Vector2F position;
    Vector2F center;
    Vector2F scale;
    float angle; // TODO: Rename to rotation
    Color tint;

    DrawParams* set_positon(float x, float y);
    DrawParams* set_center(float x, float y);
    DrawParams* set_scale(float x, float y);
    DrawParams* set_angle(float angle);
    DrawParams* set_tint(Color tint);
};

#endif
