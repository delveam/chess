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
    DrawParams() = default;

    Vector2F position() const
    {
        return m_position;
    }
    Vector2F center() const
    {
        return m_center;
    }
    Vector2F scale() const
    {
        return m_scale;
    }
    float rotation() const
    {
        return m_rotation;
    }
    Color tint() const
    {
        return m_tint;
    }

    DrawParams& set_position(float x, float y);
    DrawParams& set_center(float x, float y);
    DrawParams& set_scale(float x, float y);
    DrawParams& set_rotation(float rotation);
    DrawParams& set_tint(Color tint);
private:
    Vector2F m_position { 0, 0 };
    Vector2F m_center { 0, 0 };
    Vector2F m_scale { 1, 1 };
    float m_rotation { 0 };
    Color m_tint { 0xffffff };
};
#endif
