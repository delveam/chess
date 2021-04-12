#ifndef HEADER_HPP
#define HEADER_HPP
#include <allegro5/allegro_font.h>
#include "game.hpp"

class Chess : public Game {
	public:
		Chess();
		void initialize() override;
		void update() override;
		void draw() override;
		void destroy() override;
		~Chess();
	private:
		ALLEGRO_FONT* font;
};
#endif
