#ifndef DAM_KEBOARD_STATE_HPP
#define DAM_KEBOARD_STATE_HPP
#include <allegro5/allegro5.h>
#include <memory>
#include "key.hpp"

namespace dam {
namespace input {
class KeyboardState;
}
}

class dam::input::KeyboardState {
public:
    KeyboardState();

    std::unique_ptr<ALLEGRO_KEYBOARD_STATE> state;

    bool is_key_down(dam::input::Key key) const;
    bool is_key_up(dam::input::Key key) const;
};
#endif
