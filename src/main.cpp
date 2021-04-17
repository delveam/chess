#include "chess.hpp"
#include "allegro.hpp"

int main()
{
    Chess chess = Chess();
    Allegro allegro = Allegro();
    allegro.run(chess);

    return 0;
}
