#ifndef DAM_IMAGE_REGION
#define DAM_IMAGE_REGION
namespace dam {
    namespace graphics {
    class ImageRegion;
    }
}

class dam::graphics::ImageRegion {
    public:
        ImageRegion();
        ImageRegion(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
        unsigned int x;
        unsigned int y;
        unsigned int width;
        unsigned int height;
};
#endif
