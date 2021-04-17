#ifndef BACKEND_HPP
#define BACKEND_HPP
#include "game.hpp"

class Backend {
public:
    virtual void run(Game& game) = 0;
};
#endif
