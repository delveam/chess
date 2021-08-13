#include "allegro.hpp"
#include "chess.hpp"

int main(int argc, char* argv[])
{
    auto args = dam::args::create(argc, argv);
    auto config = dam::AppConfig()
                  .enable_resizable_window()
                  .set_title("Chess");

    auto app = Chess(args);
    auto backend = Allegro();

    backend.run(config, app);
}
