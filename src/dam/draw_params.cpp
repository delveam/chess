#include "draw_params.hpp"

dam::graphics::DrawParams& dam::graphics::DrawParams::set_position(float x, float y)
{
    m_position = Vector2F(x, y);

    return *this;
}

dam::graphics::DrawParams& dam::graphics::DrawParams::set_center(float x, float y)
{
    m_center = Vector2F(x, y);

    return *this;
}

dam::graphics::DrawParams& dam::graphics::DrawParams::set_scale(float x, float y)
{
    m_scale = Vector2F(x, y);

    return *this;
}

dam::graphics::DrawParams& dam::graphics::DrawParams::set_rotation(float rotation)
{
    m_rotation = rotation;

    return *this;
}

dam::graphics::DrawParams& dam::graphics::DrawParams::set_tint(Color tint)
{
    m_tint = tint;

    return *this;
}
