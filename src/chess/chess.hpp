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
    void event(dam::Context& ctx, dam::EventType event) override;
    void draw(dam::Context& ctx) override;
    void destroy(dam::Context& ctx) override;
    ~Chess();
private:
    dam::graphics::Font* font;
    dam::graphics::Texture* pieces;
    int square_size;
    int sprite_size;
    float sprite_scale;
    dam::Vector2F board_offset;
    Board board;
    bool board_flipped;
    bool selected;
    std::string initial_selection;
};
#endif
