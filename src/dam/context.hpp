#ifndef DAM_CONTEXT_HPP
#define DAM_CONTEXT_HPP
#include <allegro5/allegro5.h>
#include <string>
#include "smart_keyboard.hpp"
#include "smart_mouse.hpp"

namespace dam {
class Context;
}

class dam::Context {
// TODO: I want this to be "internal"... but I do not think that is possible within cpp.
public:
    Context();

    ALLEGRO_DISPLAY* display;
    unsigned int display_width;
    unsigned int display_height;
    dam::input::SmartKeyboard keyboard;
    dam::input::SmartMouse mouse;
};
#endif
