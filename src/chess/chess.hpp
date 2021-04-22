#ifndef CHESS_HPP
#define CHESS_HPP
#include <allegro5/allegro_font.h>
#include <iostream>
#include "../dam/app.hpp"
#include "../dam/dam.hpp"
#include "../dam/smart_keyboard.hpp"
#include "board.hpp"

class Chess : public dam::App {
public:
    Chess();
    void initialize() override;
    void update() override;
    void draw() override;
    void destroy() override;
    ~Chess();
private:
    ALLEGRO_FONT* font;
    ALLEGRO_BITMAP* pawn_black;
    ALLEGRO_BITMAP* bishop_black;
    ALLEGRO_BITMAP* knight_black;
    ALLEGRO_BITMAP* rook_black;
    ALLEGRO_BITMAP* queen_black;
    ALLEGRO_BITMAP* king_black;
    ALLEGRO_BITMAP* pawn_white;
    ALLEGRO_BITMAP* bishop_white;
    ALLEGRO_BITMAP* knight_white;
    ALLEGRO_BITMAP* rook_white;
    ALLEGRO_BITMAP* queen_white;
    ALLEGRO_BITMAP* king_white;
    Board board;

    std::unique_ptr<dam::SmartKeyboard> keyboard;


};
#endif
