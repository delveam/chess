// #include <allegro5/allegro5.h>
#include "game.hpp"

Game::Game() {
}

void Game::run() {
   initialize(); 
   while (!should_close()) {
       update();
       draw();
   }
    destroy();
}

Game::~Game() {
}
