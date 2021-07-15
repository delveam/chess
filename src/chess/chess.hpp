#ifndef CHESS_HPP
#define CHESS_HPP
#include <allegro5/allegro_font.h>
#include <iostream>
#include "board.hpp"
#include "match.hpp"
#include "../dam/app.hpp"
#include "../dam/graphics.hpp"

class Chess : public dam::App {
public:
    Chess() = default;

    void initialize(dam::Context& ctx) override;
    void update(dam::Context& ctx) override;
    void event(dam::Context& ctx, dam::EventType event) override;
    void draw(dam::Context& ctx) override;
    void destroy(dam::Context& ctx) override;
private:
    dam::graphics::Font* font;
    dam::graphics::Texture* pieces;
    int sprite_size { 16 };
    int square_size;
    float sprite_scale;
    dam::Vector2F board_offset;
    Match match;
    bool board_flipped { false };
    bool selected { false };

    float ai_delay_duration { 0.8 };
    Team ai_team { Team::Black };

    std::optional<Coordinates> initial_selection { std::nullopt };
    bool move_was_queued { false };
    std::optional<Move> queued_move { std::nullopt };
    float move_time { 0 };
    dam::Vector2F current_position;
    dam::Vector2F previous_position;

    float engine_delay { 0 };

    void handle_resize(dam::Context& ctx);

    void reset_selection();
    void queue_move(std::string lan);

    void update_input(dam::Context& ctx);
    void update_ai(dam::Context& ctx);
    void update_tweening(dam::Context& ctx);
};
#endif
