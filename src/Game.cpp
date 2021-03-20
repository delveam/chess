#include "Game.hpp"
#include "raylib-cpp.hpp"

Game::Game()
{
	title = "Application";
	window_width = 640;
	window_height = 360;
}

void Game::run()
{
	initialize();

	raylib::Window window(window_width, window_height, title);

	SetTargetFPS(60);

	while (!window.ShouldClose())
	{
		update();
		draw();
	}
}
