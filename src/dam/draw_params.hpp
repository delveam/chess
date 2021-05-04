#ifndef DRAW_PARAMS_HPP
#define DRAW_PARAMS_HPP
#include <tuple>
#include "color.hpp"
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
    Color color;
    std::tuple<float, float> dimension;
    DrawParams* set_positon(float x, float y);
    DrawParams* set_color(Color color);
    DrawParams* set_dimension(float width, float height);
};

#endif
