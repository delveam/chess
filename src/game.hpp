#include <allegro5/allegro_font.h>

namespace game {
  class Chess {
    public:
      Chess();
      void initialize();
      void update();
      void draw();
      ~Chess();
    private:
      ALLEGRO_FONT* font;
  };
}
