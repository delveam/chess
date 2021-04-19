#include "allegro.hpp"
#include "chess.hpp"

int main()
{
    Chess chess = Chess();
    Allegro allegro = Allegro();
    allegro.run(chess);

    return 0;
}
