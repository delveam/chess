#include "allegro.hpp"
#include "chess.hpp"

int main()
{
    auto config = dam::AppConfig()
                  .set_title("Chess");

    auto backend = Allegro();
    auto app = Chess();

    backend.run(config, app);

    return 0;
}
