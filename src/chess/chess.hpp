#ifndef CHESS_HPP
#define CHESS_HPP
#include <allegro5/allegro_font.h>
#include <iostream>
#include "../dam/app.hpp"
#include "../dam/graphics.hpp"
#include "board.hpp"

class Chess : public dam::App {
public:
    Chess();
    void initialize(dam::Context& ctx) override;
    void update(dam::Context& ctx) override;
    void draw(dam::Context& ctx) override;
    void destroy(dam::Context& ctx) override;
    ~Chess();
private:
    dam::graphics::Font* font;
    dam::graphics::Texture* pawn_black;
    dam::graphics::Texture* bishop_black;
    dam::graphics::Texture* knight_black;
    dam::graphics::Texture* rook_black;
    dam::graphics::Texture* queen_black;
    dam::graphics::Texture* king_black;
    dam::graphics::Texture* pawn_white;
    dam::graphics::Texture* bishop_white;
    dam::graphics::Texture* knight_white;
    dam::graphics::Texture* rook_white;
    dam::graphics::Texture* queen_white;
    dam::graphics::Texture* king_white;
    Board board;
    bool board_flipped;
};
#endif
