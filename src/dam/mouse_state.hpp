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
    MouseState() :
        state(std::make_unique<ALLEGRO_MOUSE_STATE>())
    {
        al_get_mouse_state(state.get());
    }

    std::unique_ptr<ALLEGRO_MOUSE_STATE> state;

    bool is_button_down(dam::input::MouseButton button) const;
    bool is_button_up(dam::input::MouseButton button) const;
    Vector2F get_position() const;
    int get_scroll_wheel_value() const;
};
#endif
