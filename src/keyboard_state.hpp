#ifndef KEBOARD_STATE_HPP
#define KEBOARD_STATE_HPP
#include <allegro5/allegro5.h>
#include <memory>

namespace dam {
class KeyboardState;
}

class dam::KeyboardState {
public:
    KeyboardState();
    std::unique_ptr<ALLEGRO_KEYBOARD_STATE> state;
    bool is_key_down(int key);
    bool is_key_up(int key);
};

#endif
