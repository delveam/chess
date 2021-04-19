#ifndef GAME_HPP
#define GAME_HPP

class Game {
public:
    Game();
    bool loop;
    virtual void initialize() = 0;
    virtual void update() = 0;
    virtual void draw() = 0;
    virtual void destroy() = 0;
    ~Game();
};
#endif
