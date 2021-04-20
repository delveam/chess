#include "rectanglef.hpp"

dam::RectangleF::RectangleF()
{
    x = 0;
    y = 0;
    width = 0;
    height = 0;
}

dam::RectangleF::RectangleF(float x, float y, float width, float height)
{
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
}
