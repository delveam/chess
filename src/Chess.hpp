#ifndef CHESS_H
#define CHESS_H

#include "Game.hpp"

class Chess : public Game
{
	public:
		void initialize() override;
		void update() override;
		void draw() override;
};

#endif
