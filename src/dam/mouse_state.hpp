#ifndef DAM_MOUSE_STATE_HPP
#define DAM_MOUSE_STATE_HPP
#include <allegro5/allegro5.h>
#include <memory>
#include "mouse_button.hpp"
#include "vector2f.hpp"

namespace dam {
namespace input {
class MouseState;
}
}

class dam::input::MouseState {
public:
    MouseState();
    std::unique_ptr<ALLEGRO_MOUSE_STATE> state;
    bool is_button_down(dam::input::MouseButton button);
    bool is_button_up(dam::input::MouseButton button);
    Vector2F get_position();
    int get_scroll_wheel_value();
};

#endif
