#include <allegro5/allegro_font.h>
#include "game.hpp"

class Chess : Game {
  public:
    Chess();
    bool should_close();
    void initialize();
    void update();
    void draw();
    void destroy();
    ~Chess();
  private:
    ALLEGRO_FONT* font;
};
