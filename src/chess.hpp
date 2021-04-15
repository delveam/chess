#ifndef HEADER_HPP
#define HEADER_HPP
#include <allegro5/allegro_font.h>
#include "game.hpp"

class Chess : public Game {
	public:
		Chess();
		void initialize(DamContext& context) override;
		void update(DamContext& context) override;
		void draw(DamContext& context) override;
		void destroy(DamContext& context) override;
		~Chess();
	private:
		ALLEGRO_FONT* font;
		ALLEGRO_BITMAP* texture;
};
#endif
