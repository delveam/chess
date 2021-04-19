#ifndef CHESS_HPP
#define CHESS_HPP
#include <allegro5/allegro_font.h>
#include <iostream>
#include "dam.hpp"
#include "game.hpp"
#include "smart_keyboard.hpp"

class Chess : public Game {
public:
    Chess();
    void initialize() override;
    void update() override;
    void draw() override;
    void destroy() override;
    ~Chess();
private:
    ALLEGRO_FONT* font;
    ALLEGRO_BITMAP* texture;
    std::unique_ptr<dam::SmartKeyboard> keyboard;
};
#endif
