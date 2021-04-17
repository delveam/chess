#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <iostream>
#include <memory>
#include "chess.hpp"
#include "matrix4f.hpp"
#include "dam.hpp"

Chess::Chess()
{
}

void Chess::initialize(DamContext& context)
{
    font = al_create_builtin_font();
    texture = context.load_texture<ALLEGRO_BITMAP*>("content/sprites/pawn.png");

    // auto a = dam::Matrix4F::create_translation(5, 6, 7);
    // auto b = dam::Matrix4F::create_scale(2, 2, 1);
    // auto temp = dam::Matrix4F::multiply(a, b);
    // temp.debug();

    // keyboard = dam::SmartKeyboard();
}

void Chess::update(DamContext& context)
{
    // keyboard.update();

    // std::cout << keyboard.previous.is_key_down(ALLEGRO_KEY_W) << " ";
    // std::cout << keyboard.current.is_key_down(ALLEGRO_KEY_W) << "\n";

    // if (sk.pressed(ALLEGRO_KEY_W)) {
    // 	std::cout << "YEET\n";
    // }
    // if (sk.pressing(ALLEGRO_KEY_T)) {
    // 	std::cout << "BRUH\n";
    // }

    ALLEGRO_KEYBOARD_STATE temp;
    al_get_keyboard_state(&temp);

    auto state = context.get_keyboard_state_bruh<ALLEGRO_KEYBOARD_STATE*>(std::any(&temp));

    if (al_key_down(&temp, ALLEGRO_KEY_ESCAPE)) {
        loop = false;
    }

    // auto state = dam::Keyboard::get_state();
    // if (state.is_key_down(ALLEGRO_KEY_ESCAPE)) {
    //     loop = false;
    // }
}

void Chess::draw(DamContext& context)
{
    ALLEGRO_TRANSFORM transform;
    al_identity_transform(&transform);
    al_use_transform(&transform);
    context.draw_rectangle(128, 128, 256, 300, al_map_rgb(255, 0, 0));

    al_rotate_transform(&transform, 3.141519 / 8);
    al_use_transform(&transform);
    context.draw_text(32, 32, "yeet", font, al_map_rgb(255, 255, 255));

    context.draw_texture(64, 64, texture);

    dam::draw_rectangle(5, 5, 126, 123, al_map_rgb(0, 255, 0));

}

void Chess::destroy(DamContext& context)
{
    al_destroy_font(font);
    context.unload_texture(texture);
}

Chess::~Chess()
{
}
