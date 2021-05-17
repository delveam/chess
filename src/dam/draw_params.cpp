#include "draw_params.hpp"

dam::graphics::DrawParams::DrawParams()
{
    position = Vector2F(0, 0);
    center = Vector2F(0, 0);
    scale = Vector2F(1, 1);
    angle = 0;
    tint = Color(0xffffff);
}

dam::graphics::DrawParams* dam::graphics::DrawParams::set_positon(float x, float y)
{
    position.x = x;
    position.y = y;

    return this;
}

dam::graphics::DrawParams* dam::graphics::DrawParams::set_center(float x, float y)
{
    center.x = x;
    center.y = y;

    return this;
}

dam::graphics::DrawParams* dam::graphics::DrawParams::set_scale(float x, float y)
{
    scale.x = x;
    scale.y = y;

    return this;
}

dam::graphics::DrawParams* dam::graphics::DrawParams::set_angle(float angle)
{
    this->angle = angle;
    return this;
}

dam::graphics::DrawParams* dam::graphics::DrawParams::set_tint(Color tint)
{
    this->tint = tint;

    return this;
}
