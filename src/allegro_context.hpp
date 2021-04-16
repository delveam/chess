#ifndef ALLEGRO_CONTEXT_HPP
#define ALLEGRO_CONTEXT_HPP
#include "dam_context.hpp"
#include <any>

class AllegroContext : public DamContext {
	protected:
		std::any load_texture_impl(std::string path) override;
		std::any load_font_impl(std::string path) override;
		void draw_rectangle_impl(float x, float y, float width, float height, std::any color) override;
		void draw_texture_impl(float x, float y, std::any texture) override;
		void draw_text_impl(float x, float y, std::string text, std::any font, std::any color) override;
		void unload_texture_impl(std::any texture) override;
		void unload_font_impl(std::any font) override;
		std::any get_keyboard_state_impl() override;
};
#endif
