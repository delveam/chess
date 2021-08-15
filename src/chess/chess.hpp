#ifndef CHESS_HPP
#define CHESS_HPP
#include "board.hpp"
#include "engine.hpp"
#include "match.hpp"
#include "renderable.hpp"
#include "tweenable.hpp"
#include "../dam/app.hpp"
#include "../dam/graphics.hpp"

class Chess : public dam::App {
public:
    Chess() = default;
    Chess(dam::Args args) : dam::App(args)
    {
    }

    void initialize(dam::Context& ctx) override;
    void update(dam::Context& ctx) override;
    void event(dam::Context& ctx, dam::EventType event) override;
    void draw(dam::Context& ctx) override;
    void destroy(dam::Context& ctx) override;
private:
    dam::graphics::Font* m_font;
    dam::graphics::Texture* m_pieces;
    bool m_loaded_font { false };

    int m_sprite_size { 32 };
    int m_square_size;
    float m_sprite_scale;
    dam::Vector2F m_board_offset;
    Match m_match { Match::create() };
    bool m_board_flipped { false };
    bool m_selected { false };

    bool m_debug_danger_zone { false };

    dam::graphics::Color m_board_light_color { dam::graphics::Color(0xdee3e6) };
    dam::graphics::Color m_board_dark_color { dam::graphics::Color(0x8ca2ad) };
    dam::graphics::Color m_board_selection_color { dam::graphics::Color(0x547c64) };

    bool m_ai_enabled { true };
    unsigned int m_search_depth { 2 };
    Team ai_team { Team::Black };

    engine::Pescado m_uci;
    bool m_uci_ok { false };
    bool m_uci_ready { false };
    bool m_uci_setup { false };
    bool m_sent_go_command { false };
    std::queue<std::string> m_commands;

    std::optional<Coordinates> m_initial_selection { std::nullopt };
    bool m_move_was_queued { false };
    std::optional<Move> m_queued_move { std::nullopt };

    float m_move_duration { 0.25 };
    Tweenable m_primary_mover;
    std::optional<Tweenable> m_secondary_mover;
    int m_rook_index { 0 };

    Renderable m_primary_renderable;
    Renderable m_secondary_renderable;

    std::array<PieceType, 4> m_promotion_pieces { PieceType::Queen, PieceType::Knight, PieceType::Rook, PieceType::Bishop };
    bool m_promoting { false };
    bool m_promoting_debounce { false };
    std::optional<std::string> m_promotion_lan { std::nullopt };
    std::optional<unsigned int> m_promotion_file { std::nullopt };
    std::optional<Team> m_promotion_team { std::nullopt };

    float engine_delay_timer { 0 };

    void handle_resize(dam::Context& ctx);

    void reset_selection();
    void reset_promotion();

    void queue_move(std::string lan);
    void submit_move(std::string lan);

    void update_input(dam::Context& ctx);
    void update_promotion(dam::Context& ctx);
    void update_ai();
    void update_tweening(dam::Context& ctx);

    void on_event(std::string event);
    void queue_command(std::string command);
    void update_uci();

    dam::Vector2F calculate_draw_position(float x, float y);
    std::optional<dam::graphics::ImageRegion> image_region_from_piece(Piece piece);

    void draw_board(dam::Context& ctx);
    void draw_coordinates(dam::Context& ctx);
    void draw_danger_zone(dam::Context& ctx);
    void draw_recent_move_indicator(dam::Context& ctx);
    void draw_king_safety(dam::Context& ctx);
    void draw_selection(dam::Context& ctx);
    void draw_moves(dam::Context& ctx);
    void draw_pieces(dam::Context& ctx);
    void draw_move_animation(dam::Context& ctx);
    void draw_promotion(dam::Context& ctx);
};
#endif
