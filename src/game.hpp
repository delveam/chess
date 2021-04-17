#ifndef GAME_HPP
#define GAME_HPP
#include "dam_context.hpp"

class Game {
public:
    Game();
    bool loop;
    virtual void initialize(DamContext& context) = 0;
    virtual void update(DamContext& context) = 0;
    virtual void draw(DamContext& context) = 0;
    virtual void destroy(DamContext& context) = 0;
    ~Game();
};
#endif
