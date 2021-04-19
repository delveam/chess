#ifndef BACKEND_HPP
#define BACKEND_HPP
#include "game.hpp"

namespace dam {
class Backend;
}

class dam::Backend {
public:
    virtual void run(Game& game) = 0;
};
#endif
