#ifndef CHESS_HPP
#define CHESS_HPP
#include <allegro5/allegro_font.h>
#include "game.hpp"
#include "smart_keyboard.hpp"

class Chess : public Game {
public:
    Chess();
    void initialize(DamContext& context) override;
    void update(DamContext& context) override;
    void draw(DamContext& context) override;
    void destroy(DamContext& context) override;
    ~Chess();
private:
    ALLEGRO_FONT* font;
    ALLEGRO_BITMAP* texture;
    dam::SmartKeyboard* yeeter;
};
#endif
