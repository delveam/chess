#include "draw_params.hpp"

dam::graphics::DrawParams::DrawParams()
{
    position = Vector2F(0, 0);
    color = Color(0xffffff);
    dimension = std::make_tuple(0, 0);
}

dam::graphics::DrawParams* dam::graphics::DrawParams::set_positon(float x, float y)
{
    position.x = x;
    position.y = y;

    return this;
}

dam::graphics::DrawParams* dam::graphics::DrawParams::set_color(Color color)
{
    this->color = color;

    return this;
}

dam::graphics::DrawParams* dam::graphics::DrawParams::set_dimension(float width, float height)
{
    std::get<0>(dimension) = width;
    std::get<1>(dimension) = height;

    return this;
}
