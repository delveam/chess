#include "image_region.hpp"

dam::graphics::ImageRegion::ImageRegion()
{
    this->x = 0;
    this->y = 0;
    this->width = 0;
    this->height = 0;
}

dam::graphics::ImageRegion::ImageRegion(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
}
