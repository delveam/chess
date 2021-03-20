#ifndef GAME_H
#define GAME_H

#include <string>

class Game
{
	public:
		std::string title;
		int window_width;
		int window_height;

		Game();
		void run();

		virtual void initialize() = 0;
		virtual void update() = 0;
		virtual void draw() = 0;
};

#endif
