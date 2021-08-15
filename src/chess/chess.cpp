#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <iostream>
#include <memory>
#include <regex>
#include "chess.hpp"
#include "coordinates.hpp"
#include "engine.hpp"
#include "utils.hpp"
#include "../dam/input.hpp"
#include "../dam/palette.hpp"
#include "../dam/window.hpp"

std::optional<CastlingRights> lan_as_castling_rights(std::string lan)
{
    // This does not check whether or not the current piece is even a King!

    if (lan == "e1g1") {
        return CastlingRights::WhiteKingSide;
    }
    if (lan == "e1c1") {
        return CastlingRights::WhiteQueenSide;
    }
    if (lan == "e8g8") {
        return CastlingRights::BlackKingSide;
    }
    if (lan == "e8c8") {
        return CastlingRights::BlackQueenSide;
    }

    return std::nullopt;
}

void Chess::handle_resize(dam::Context& ctx)
{
    m_square_size = dam::window::get_height(ctx) / constants::board_height;

    if (m_square_size * constants::board_width > (int)dam::window::get_width(ctx)) {
        m_square_size = dam::window::get_width(ctx) / constants::board_width;
    }

    m_board_offset = dam::Vector2F(
                         (dam::window::get_width(ctx) - (m_square_size * constants::board_width)) / 2,
                         (dam::window::get_height(ctx) - (m_square_size * constants::board_height)) / 2
                     );

    m_sprite_scale = (float)m_square_size / m_sprite_size;

    if (m_loaded_font) {
        dam::graphics::unload_font(m_font);
    }

    m_font = dam::graphics::load_font("./content/fonts/FiraCode-SemiBold.ttf", m_square_size * 0.22);
    m_loaded_font = true;
}

void Chess::reset_selection()
{
    m_selected = false;
    m_initial_selection = std::nullopt;
}

void Chess::reset_promotion()
{
    m_promoting = false;
    m_promoting_debounce = false;
    m_promotion_lan = std::nullopt;
    m_promotion_file = std::nullopt;
    m_promotion_team = std::nullopt;
}

void Chess::queue_move(std::string lan)
{
    auto move = Move::create(lan);

    if (!move.has_value()) {
        return;
    }

    auto move_start_index = move->start().y() * constants::board_width + move->start().x();

    std::optional<CastlingRights> castling_rights = std::nullopt;

    if (m_match.board().pieces().at(move_start_index).type() == PieceType::King) {
        castling_rights = lan_as_castling_rights(move->lan());
    }

    if (castling_rights.has_value()) {
        dam::Vector2F king_start;
        dam::Vector2F king_end;
        dam::Vector2F rook_start;
        dam::Vector2F rook_end;

        switch (castling_rights.value()) {
        case CastlingRights::WhiteKingSide: {
            king_start = dam::Vector2F(4, 7);
            king_end = dam::Vector2F(6, 7);
            rook_start = dam::Vector2F(7, 7);
            rook_end = dam::Vector2F(5, 7);
            break;
        }
        case CastlingRights::WhiteQueenSide: {
            king_start = dam::Vector2F(4, 7);
            king_end = dam::Vector2F(2, 7);
            rook_start = dam::Vector2F(0, 7);
            rook_end = dam::Vector2F(3, 7);
            break;
        }
        case CastlingRights::BlackKingSide: {
            king_start = dam::Vector2F(4, 0);
            king_end = dam::Vector2F(6, 0);
            rook_start = dam::Vector2F(7, 0);
            rook_end = dam::Vector2F(5, 0);
            break;
        }
        case CastlingRights::BlackQueenSide: {
            king_start = dam::Vector2F(4, 0);
            king_end = dam::Vector2F(2, 0);
            rook_start = dam::Vector2F(0, 0);
            rook_end = dam::Vector2F(3, 0);
            break;
        }
        default:
            break;
        }

        m_move_was_queued = true;
        m_queued_move = move;

        m_rook_index = rook_start.y() * constants::board_width + rook_start.x();

        m_primary_renderable.set(king_start);

        m_primary_mover = Tweenable(king_start, king_end, m_move_duration);
        m_secondary_mover = Tweenable(rook_start, rook_end, m_move_duration);

        return;
    }

    m_move_was_queued = true;
    m_queued_move = move;

    m_primary_renderable.set(dam::Vector2F(move->start().x(), move->start().y()));

    auto start = dam::Vector2F(move->start().x(), move->start().y());
    auto end = dam::Vector2F(move->end().x(), move->end().y());

    m_primary_mover = Tweenable(start, end, m_move_duration);
}

void Chess::submit_move(std::string lan)
{
    m_move_was_queued = false;
    m_match.submit_move(lan);
    reset_selection();

    queue_command("position fen " + Board::into_fen(m_match.board()));
    m_sent_go_command = false;
}

void Chess::update_input(dam::Context& ctx)
{
    using namespace dam::input;

    if (Keyboard::pressed(ctx, Key::Escape)) {
        std::cout << "\n" << m_match.get_moves() << "\n";
        loop = false;
    }

    if ( Keyboard::pressed(ctx, Key::F)) {
        m_board_flipped = !m_board_flipped;
    }

    if (Keyboard::pressed(ctx, Key::F1)) {
        m_debug_danger_zone = !m_debug_danger_zone;
    }

    if (!m_promoting) {
        if (Mouse::get_scroll_stride(ctx) > 0) {
            m_match.redo();
            reset_selection();
        }
        if (Mouse::get_scroll_stride(ctx) < 0) {
            m_match.undo();
            reset_selection();
        }
    }

    if (m_selected && Mouse::pressed(ctx, MouseButton::Right)) {
        reset_selection();
    }

    if (m_move_was_queued) {
        return;
    }

    if (Mouse::pressed(ctx, MouseButton::Left)) {
        if (!m_selected) {
            auto position = Mouse::get_position(ctx);
            auto x = (int)((position.x() - m_board_offset.x()) / m_square_size);
            auto y = (int)((position.y() - m_board_offset.y()) / m_square_size);

            if (m_board_flipped) {
                x = constants::board_width - x - 1;
                y = constants::board_height - y - 1;
            }

            auto target = m_match.board().get(x, y);

            if (!target.has_value() || target->team() != m_match.board().current_team()) {
                return;
            }

            m_initial_selection = Coordinates::create(x, y);
            m_selected = true;
        }
        else {
            auto position = Mouse::get_position(ctx);
            auto x = (int)((position.x() - m_board_offset.x()) / m_square_size);
            auto y = (int)((position.y() - m_board_offset.y()) / m_square_size);

            if (m_board_flipped) {
                x = constants::board_width - x - 1;
                y = constants::board_height - y - 1;
            }

            auto target = m_match.board().get(x, y);

            if (!target.has_value()) {
                return;
            }

            // Quickly change to another piece.
            if (target->team() == m_match.board().current_team()) {
                m_initial_selection = Coordinates::create(x, y);
                return;
            }

            auto coords = Coordinates::create(x, y).value();
            auto lan = m_initial_selection->to_string() + coords.to_string();

            auto initial_piece = m_match.board().get(m_initial_selection->x(), m_initial_selection->y()).value();

            if (initial_piece.type() == PieceType::Pawn) {
                auto start_y = 0;
                auto end_y = 0;

                switch (m_match.board().current_team()) {
                case Team::White: {
                    start_y = 1;
                    end_y = 0;
                    break;
                }
                case Team::Black: {
                    start_y = 6;
                    end_y = 7;
                    break;
                }
                default:
                    break;
                }

                // Check if we are about to promote a pawn.
                if ((int)m_initial_selection->y() == start_y && y == end_y) {
                    // Note that this is just a dummy lan; the player will get to choose what piece to promote to once the move is complete.
                    lan += "q";
                }
            }

            if (m_match.analysis().contains_move(lan)) {
                queue_move(lan);
            }
            else {
                reset_selection();
            }
        }
    }
}

void Chess::update_promotion(dam::Context& ctx)
{
    if (!m_promoting) {
        return;
    }

    if (m_promoting_debounce && !dam::input::Mouse::pressing(ctx, dam::input::MouseButton::Left)) {
        m_promoting_debounce = false;
    }

    if (m_promoting_debounce) {
        return;
    }

    auto mouse_position = dam::input::Mouse::get_position(ctx);
    auto mouse_x = (int)((mouse_position.x() - m_board_offset.x()) / m_square_size);
    auto mouse_y = (int)((mouse_position.y() - m_board_offset.y()) / m_square_size);

    if (m_board_flipped) {
        mouse_x = constants::board_width - mouse_x - 1;
        mouse_y = constants::board_height - mouse_y - 1;
    }

    if (mouse_x == m_promotion_file) {
        if ((m_promotion_team == Team::White && mouse_y < (int)m_promotion_pieces.size()) || (m_promotion_team == Team::Black && mouse_y >= constants::board_height - (int)m_promotion_pieces.size())) {
            if (dam::input::Mouse::pressed(ctx, dam::input::MouseButton::Left)) {

                auto promotion = "q";

                switch (m_promotion_team.value()) {
                case Team::White: {
                    switch (mouse_y) {
                    case 0: {
                        promotion = "q";
                        break;
                    }
                    case 1: {
                        promotion = "n";
                        break;
                    }
                    case 2: {
                        promotion = "r";
                        break;
                    }
                    case 3: {
                        promotion = "b";
                        break;
                    }
                    }
                    break;
                }
                case Team::Black: {
                    switch (mouse_y) {
                    case 4: {
                        promotion = "b";
                        break;
                    }
                    case 5: {
                        promotion = "r";
                        break;
                    }
                    case 6: {
                        promotion = "n";
                        break;
                    }
                    case 7: {
                        promotion = "q";
                        break;
                    }
                    }
                    break;
                }
                default:
                    break;
                }

                submit_move(m_promotion_lan.value() + promotion);
                reset_promotion();
            }
        }
    }
}

void Chess::update_ai()
{
    if (!m_ai_enabled) {
        return;
    }

    if (!m_uci_ready) {
        return;
    }

    if (m_sent_go_command) {
        return;
    }

    if (!m_match.at_end() || m_match.board().current_team() != ai_team) {
        return;
    }

    if (gm::end_condition(m_match.analysis().king_safety())) {
        return;
    }

    queue_command("go depth " + std::to_string(m_search_depth));
    m_sent_go_command = true;
}

void Chess::update_tweening(dam::Context& ctx)
{
    if (!m_move_was_queued || m_promoting) {
        return;
    }

    m_primary_mover.update(ctx.delta_time);
    m_primary_renderable.set(m_primary_mover.position());

    if (m_secondary_mover.has_value()) {
        m_secondary_mover->update(ctx.delta_time);
        m_secondary_renderable.set(m_secondary_mover->position());
    }

    if (m_primary_mover.done()) {
        m_secondary_mover = std::nullopt;

        // If the AI made a move then simply submit the move.
        if (m_ai_enabled && m_match.board().current_team() == ai_team) {
            submit_move(m_queued_move->lan());

            return;
        }

        // If the player is about to promote a piece then enable the promotion UI.
        if (m_queued_move->promotion().has_value()) {
            m_promoting = true;
            m_promoting_debounce = true;
            m_promotion_lan = m_queued_move->lan().substr(0, 4);
            m_promotion_file = m_promotion_lan->substr(2, 1).c_str()[0] - 'a';
            m_promotion_team = m_match.board().current_team();

            return;
        }

        // We are not dealing with anything fancy at this point; just submit the move.
        submit_move(m_queued_move->lan());
    }
}

void Chess::update_uci()
{
    while (m_commands.size() > 0) {
        auto command = m_commands.front();

        std::cout << "==> " << command << "\n";

        if (command == "isready") {
            m_uci_ready = false;
        }

        m_uci.submit(command);

        m_commands.pop();
    }

    m_uci.update();
}

dam::Vector2F Chess::calculate_draw_position(float x, float y)
{
    auto draw_x = x;
    auto draw_y = y;

    if (m_board_flipped) {
        draw_x = constants::board_width - draw_x - 1;
        draw_y = constants::board_height - draw_y - 1;
    }

    draw_x = draw_x * m_square_size + m_board_offset.x();
    draw_y = draw_y * m_square_size + m_board_offset.y();

    return dam::Vector2F(draw_x, draw_y);
}

std::optional<dam::graphics::ImageRegion> Chess::image_region_from_piece(Piece piece)
{
    auto subregion_x = 0;

    switch (piece.type()) {
    case PieceType::Pawn:
        break;
    case PieceType::Bishop:
        subregion_x = m_sprite_size;
        break;
    case PieceType::Knight:
        subregion_x = m_sprite_size * 2;
        break;
    case PieceType::Rook:
        subregion_x = m_sprite_size * 3;
        break;
    case PieceType::Queen:
        subregion_x = m_sprite_size * 4;
        break;
    case PieceType::King:
        subregion_x = m_sprite_size * 5;
        break;
    case PieceType::None:
        return std::nullopt;
    }

    return dam::graphics::ImageRegion(
               subregion_x,
               piece.team() == Team::White ? 0 : m_sprite_size,
               m_sprite_size,
               m_sprite_size
           );
}

void Chess::draw_board(dam::Context& ctx)
{
    for (int y = 0; y < constants::board_height; ++y) {
        for (int x = 0; x < constants::board_width; ++x) {
            auto color = (x + y) % 2 == 0 ? m_board_light_color : m_board_dark_color;

            auto params = dam::graphics::DrawParams()
                          .set_position(m_board_offset.x() + x * m_square_size, m_board_offset.y() + y * m_square_size)
                          .set_scale(m_square_size, m_square_size)
                          .set_tint(color);

            draw_rectangle(ctx, params);
        }
    }
}

void Chess::draw_coordinates(dam::Context& ctx)
{
    for (int x = 0; x < constants::board_width; ++x) {
        auto color = x % 2 == 0 ? m_board_light_color : m_board_dark_color;
        auto temp = !m_board_flipped ? 'a' + x : 'a' + (constants::board_width - 1 - x);
        std::string text = "";
        text.push_back(temp);

        auto params = dam::graphics::DrawParams()
                      .set_position(m_board_offset.x() + x * m_square_size + m_square_size * 0.02, m_board_offset.y() + (constants::board_height - 1) * m_square_size + m_square_size * 0.76)
                      .set_tint(color);

        draw_text(ctx, text, m_font, params);
    }
    for (int y = 0; y < constants::board_height; ++y) {
        auto color = y % 2 == 0 ? m_board_light_color : m_board_dark_color;
        auto temp = !m_board_flipped ? '1' + (constants::board_height - 1 - y) : '1' + y;
        std::string text = "";
        text.push_back(temp);

        auto params = dam::graphics::DrawParams()
                      .set_position(m_board_offset.x() + (constants::board_width - 1) * m_square_size + m_square_size * 0.85, m_board_offset.y() + y * m_square_size - m_square_size * 0.01)
                      .set_tint(color);

        draw_text(ctx, text, m_font, params);
    }
}

void Chess::draw_danger_zone(dam::Context& ctx)
{
    if (!m_debug_danger_zone) {
        return;
    }

    auto danger_zone = m_match.analysis().danger_zone();

    for (int y = 0; y < constants::board_height; ++y) {
        for (int x = 0; x < constants::board_height; ++x) {
            if (danger_zone[y * constants::board_height + x]) {
                auto params = dam::graphics::DrawParams()
                              .set_position(m_board_offset.x() + x * m_square_size, m_board_offset.y() + y * m_square_size)
                              .set_scale(m_square_size, m_square_size)
                              .set_tint(dam::graphics::Color(0xff0000, 0.3));

                draw_rectangle(ctx, params);
            }
        }
    }
}

void Chess::draw_recent_move_indicator(dam::Context& ctx)
{
    using namespace dam::graphics;

    auto start_x = m_match.last_move().start().x();
    auto start_y = m_match.last_move().start().y();
    auto end_x = m_match.last_move().end().x();
    auto end_y = m_match.last_move().end().y();

    if (m_move_was_queued) {
        start_x = m_queued_move->start().x();
        start_y = m_queued_move->start().y();
        end_x = m_queued_move->end().x();
        end_y = m_queued_move->end().y();
    }

    // This is sort of a hack to get around drawing a nullmove...
    if (start_x == end_x && start_y == end_y) {
        return;
    }

    auto start_draw_position = calculate_draw_position(start_x, start_y);
    auto end_draw_position = calculate_draw_position(end_x, end_y);

    auto start_params = DrawParams()
                        .set_position(start_draw_position.x(), start_draw_position.y())
                        .set_scale(m_square_size, m_square_size)
                        .set_tint(Color(0xffff00, 0.3));
    auto end_params = DrawParams()
                      .set_position(end_draw_position.x(), end_draw_position.y())
                      .set_scale(m_square_size, m_square_size)
                      .set_tint(Color(0xffff00, 0.3));

    draw_rectangle(ctx, start_params);
    draw_rectangle(ctx, end_params);
}

// Draw an indicator around the King when they are being attacked.
void Chess::draw_king_safety(dam::Context& ctx)
{
    // TODO(thismarvin): This is so ugly... can someone make this a method or something?
    if (m_match.analysis().king_safety() != gm::KingSafety::Check && m_match.analysis().king_safety() != gm::KingSafety::Checkmate) {
        return;
    }

    auto x = m_match.analysis().king_location().x();
    auto y = m_match.analysis().king_location().y();

    auto draw_position = calculate_draw_position(x, y);

    auto temp_params = dam::graphics::DrawParams()
                       .set_position(draw_position.x(), draw_position.y())
                       .set_scale(m_square_size, m_square_size)
                       .set_tint(dam::graphics::Color(0xff0000, 0.4));

    draw_rectangle_but_not_filled(ctx, temp_params, m_square_size * 0.25);
}

void Chess::draw_selection(dam::Context& ctx)
{
    if (!m_selected || m_move_was_queued) {
        return;
    }

    auto draw_position = calculate_draw_position(m_initial_selection->x(), m_initial_selection->y());

    auto params = dam::graphics::DrawParams()
                  .set_position(draw_position.x(), draw_position.y())
                  .set_scale(m_square_size, m_square_size)
                  .set_tint(dam::graphics::Color(m_board_selection_color, 0.8));

    draw_rectangle(ctx, params);
}

// Draw an indicator for every possible square the current piece can move to.
void Chess::draw_moves(dam::Context& ctx)
{
    using namespace dam::graphics;

    if (!m_initial_selection.has_value() || m_move_was_queued) {
        return;
    }

    auto index = m_initial_selection->y() * constants::board_width + m_initial_selection->x();
    auto target_move_set = m_match.analysis().moves().at(index);

    auto mouse_position = dam::input::Mouse::get_position(ctx);
    auto mouse_x = (int)((mouse_position.x() - m_board_offset.x()) / m_square_size);
    auto mouse_y = (int)((mouse_position.y() - m_board_offset.y()) / m_square_size);

    std::optional<int> duplicate = std::nullopt;

    for (const auto& value : target_move_set) {
        auto temp_coords = value.substr(2, 2);
        auto x = temp_coords.c_str()[0] - 'a';
        auto y = constants::board_height - std::stoi(temp_coords.substr(1, 1));

        // The following is necessary in order to only draw one move indicator instead of one for each promotion.
        // I will admit the solution is hacky, but it works!
        auto temp = y * constants::board_width + x;

        if (duplicate == temp) {
            continue;
        }

        duplicate = temp;

        // This could be simplified with calculate_draw_position, but I do not know how to handle to mouse_hover part...
        auto temp_draw_x = x;
        auto temp_draw_y = y;

        if (m_board_flipped) {
            temp_draw_x = constants::board_width - x - 1;
            temp_draw_y = constants::board_height - y - 1;
        }

        auto mouse_hover = mouse_x == temp_draw_x && mouse_y == temp_draw_y;

        temp_draw_x = temp_draw_x * m_square_size + m_board_offset.x();
        temp_draw_y = temp_draw_y * m_square_size + m_board_offset.y();

        auto target_piece = m_match.board().get(x, y);

        if (target_piece.has_value() && target_piece->team() == Team::None) {
            auto radius = m_square_size * 0.35 * 0.5;
            auto alpha = 0.8;

            if (mouse_hover) {
                radius *= 1.1;
                alpha = 0.95;
            }

            auto temp_params = DrawParams()
                               .set_position(temp_draw_x + m_square_size * 0.5 - radius, temp_draw_y + m_square_size * 0.5 - radius)
                               .set_scale(radius * 2, radius * 2)
                               .set_tint(Color(m_board_selection_color, alpha));

            draw_circle(ctx, temp_params);
        }
        else {
            auto line_width = m_square_size * 0.25;
            auto alpha = 0.2;

            if (mouse_hover) {
                line_width *= 1.2;
                alpha = 0.3;
            }

            auto temp_params = DrawParams()
                               .set_position(temp_draw_x, temp_draw_y)
                               .set_scale(m_square_size, m_square_size)
                               .set_tint(Color(0xff0000, alpha));

            draw_rectangle_but_not_filled(ctx, temp_params, line_width);
        }
    }
}

void Chess::draw_pieces(dam::Context& ctx)
{
    for (int y = 0; y < constants::board_height; ++y) {
        for (int x = 0; x < constants::board_width; ++x) {
            // Do not draw the piece that is currently moving!
            if (m_move_was_queued) {
                int temp_x = m_queued_move->start().x();
                int temp_y = m_queued_move->start().y();

                if (m_board_flipped) {
                    temp_x = constants::board_width - temp_x - 1;
                    temp_y = constants::board_width - temp_y - 1;
                }

                if (x == temp_x && y == temp_y) {
                    continue;
                }

                if (m_secondary_mover.has_value()) {
                    auto temp = m_board_flipped ? constants::board_size - m_rook_index - 1 : m_rook_index;

                    if (y * constants::board_width + x == temp) {
                        continue;
                    }
                }
            }

            auto index = m_board_flipped ? ((constants::board_height - 1) - y) * constants::board_width + ((constants::board_width - 1) - x) : y * constants::board_width + x;
            auto current = m_match.board().pieces()[index];

            if (current.type() != PieceType::None) {
                auto params = dam::graphics::DrawParams()
                              .set_position(m_board_offset.x() + x * m_square_size, m_board_offset.y() + y * m_square_size)
                              .set_scale(m_sprite_scale, m_sprite_scale);
                auto region = image_region_from_piece(current).value();

                draw_texture(ctx, m_pieces, region, params);
            }
        }
    }
}

void Chess::draw_move_animation(dam::Context& ctx)
{
    if (!m_move_was_queued) {
        return;
    }

    if (m_secondary_mover.has_value()) {
        auto temp = m_secondary_renderable.interpolate(ctx.alpha);

        auto draw_position = calculate_draw_position(temp.x(), temp.y());

        auto piece = m_match.board().pieces()[m_rook_index];

        auto region = image_region_from_piece(piece).value();
        auto params = dam::graphics::DrawParams()
                      .set_position(draw_position.x(), draw_position.y())
                      .set_scale(m_sprite_scale, m_sprite_scale);

        draw_texture(ctx, m_pieces, region, params);
    }

    auto temp = m_primary_renderable.interpolate(ctx.alpha);

    auto draw_position = calculate_draw_position(temp.x(), temp.y());

    auto temp_x = m_queued_move->start().x();
    auto temp_y = m_queued_move->start().y();

    auto index = temp_y * constants::board_width + temp_x;
    auto piece = m_match.board().pieces()[index];

    auto region = image_region_from_piece(piece).value();
    auto params = dam::graphics::DrawParams()
                  .set_position(draw_position.x(), draw_position.y())
                  .set_scale(m_sprite_scale, m_sprite_scale);

    draw_texture(ctx, m_pieces, region, params);
}

void Chess::draw_promotion(dam::Context& ctx)
{
    if (!m_promoting) {
        return;
    }

    auto promotion_background_x = m_board_offset.x() + m_square_size * m_promotion_file.value();
    auto promotion_background_y = m_board_offset.y();

    if (m_promotion_team == Team::Black) {
        promotion_background_y += m_square_size * (constants::board_height - (int)m_promotion_pieces.size());
    }

    if (m_board_flipped) {
        promotion_background_x = m_board_offset.x() + m_square_size * (constants::board_width - m_promotion_file.value() - 1);
        promotion_background_y = dam::window::get_height(ctx) - m_board_offset.y() - m_square_size * (int)m_promotion_pieces.size();

        if (m_promotion_team == Team::Black) {
            promotion_background_y = m_board_offset.y();
        }
    }

    // draw opaque square around board.
    auto shadow_offset = m_square_size * 0.1;
    auto params_shadow = dam::graphics::DrawParams()
                         .set_position(promotion_background_x + shadow_offset, promotion_background_y + shadow_offset)
                         .set_scale(m_square_size, m_square_size * (int)m_promotion_pieces.size())
                         .set_tint(dam::graphics::Color(0x000000, 0.1));

    draw_rectangle(ctx, params_shadow);

    auto params_background = dam::graphics::DrawParams()
                             .set_position(m_board_offset.x(), m_board_offset.y())
                             .set_scale(m_square_size * constants::board_width, m_square_size * constants::board_height)
                             .set_tint(dam::graphics::Color(0x000000, 0.4));

    draw_rectangle(ctx, params_background);

    // draw regular background around promotion file
    auto params_file = dam::graphics::DrawParams()
                       .set_position(promotion_background_x, promotion_background_y)
                       .set_scale(m_square_size, m_square_size * (int)m_promotion_pieces.size())
                       .set_tint(m_board_light_color);

    params_file.set_position(promotion_background_x, promotion_background_y);

    draw_rectangle(ctx, params_file);

    // draw some sort of indicator around the piece that the mouse is over
    auto mouse_position = dam::input::Mouse::get_position(ctx);
    auto mouse_x = (int)((mouse_position.x() - m_board_offset.x()) / m_square_size);
    auto mouse_y = (int)((mouse_position.y() - m_board_offset.y()) / m_square_size);

    if (m_board_flipped) {
        mouse_x = constants::board_width - mouse_x - 1;
        mouse_y = constants::board_height - mouse_y - 1;
    }

    if (mouse_x == m_promotion_file) {
        if ((m_promotion_team == Team::White && mouse_y < (int)m_promotion_pieces.size()) || (m_promotion_team == Team::Black && mouse_y >= constants::board_height - (int)m_promotion_pieces.size())) {
            auto params_selection = dam::graphics::DrawParams()
                                    .set_position(promotion_background_x, promotion_background_y + mouse_y * m_square_size)
                                    .set_scale(m_square_size, m_square_size)
                                    .set_tint(dam::graphics::Color(m_board_selection_color, 0.5));

            if (m_promotion_team == Team::Black) {
                params_selection.set_position(promotion_background_x, promotion_background_y + (mouse_y - (int)m_promotion_pieces.size()) * m_square_size);
            }

            if (m_board_flipped) {
                switch (m_promotion_team.value()) {
                case Team::White: {
                    params_selection.set_position(promotion_background_x, promotion_background_y + ((int)m_promotion_pieces.size() - mouse_y - 1) * m_square_size);
                    break;
                }
                case Team::Black: {
                    params_selection.set_position(promotion_background_x, promotion_background_y + (constants::board_height - mouse_y - 1) * m_square_size);
                    break;
                }
                default:
                    break;
                }
            }

            draw_rectangle(ctx, params_selection);
        }
    }

    // draw four pieces in the center
    for (int i = 0; i < (int)m_promotion_pieces.size(); ++i) {
        auto params = dam::graphics::DrawParams()
                      .set_position(promotion_background_x, promotion_background_y + m_square_size * i)
                      .set_scale(m_sprite_scale, m_sprite_scale);
        auto region = m_promotion_team == Team::White
                      ? image_region_from_piece(Piece(m_promotion_pieces.at(i), m_promotion_team.value())).value()
                      : image_region_from_piece(Piece(m_promotion_pieces.at(m_promotion_pieces.size() - i - 1), m_promotion_team.value())).value();

        if (m_board_flipped) {
            region = m_promotion_team == Team::White
                     ? image_region_from_piece(Piece(m_promotion_pieces.at(m_promotion_pieces.size() - i - 1), m_promotion_team.value())).value()
                     : image_region_from_piece(Piece(m_promotion_pieces.at(i), m_promotion_team.value())).value();
        }

        draw_texture(ctx, m_pieces, region, params);
    }
}

void Chess::on_event(std::string event)
{
    auto args = utils::split_whitespace(event);

    if (!m_uci_ok && args.at(0) == "uciok") {
        m_uci_ok = true;
        queue_command("ucinewgame");
        queue_command("isready");
    }

    if (!m_uci_ready && args.at(0) == "readyok") {
        m_uci_ready = true;
    }

    if(!m_uci_setup && m_uci_ready) {
        m_uci_setup = true;
    }

    if (args.at(0) == "bestmove") {
        queue_move(args.at(1));
    }

    std::cout << "<--   " << event << "\n";
}

void Chess::queue_command(std::string command)
{
    m_commands.push(command);
}

void Chess::initialize(dam::Context& ctx)
{
    m_pieces = dam::graphics::load_texture("./content/sprites/pieces.png");

    handle_resize(ctx);

    srand(time(NULL));

    m_uci.subscribe([&](std::string event) {
        on_event(event);
    });

    static const auto regex_digit = std::regex("^\\d$");

    for (int i = 0; i < (int)args().size(); ++i) {
        if (args().at(i) == "--depth" && i != (int)args().size() - 1) {

            auto arg = args().at(i + 1);

            if (std::regex_match(arg, regex_digit)) {
                m_search_depth = std::stoi(arg);
            }
        }
    }

    // auto perft_tests = std::vector<std::pair<std::string, int>> {
    //     std::make_pair("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 5),
    //     std::make_pair("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 5),
    //     std::make_pair("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 7),
    //     std::make_pair("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 5),
    //     std::make_pair("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 5),
    //     std::make_pair("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 5),
    // };

    // for (const auto& test : perft_tests) {
    //     auto board = Board::create(test.first).value();
    //     auto depth = test.second;

    //     std::cout << "Board: " << test.first << "\n";

    //     for (int i = 1; i <= depth; ++i) {
    //         auto total = engine::Pescado::calculate_total_moves(board, i);

    //         std::cout << "  Depth " << i << ": " << total << "\n";
    //     }

    //     std::cout << "\n";
    // }

    queue_command("uci");
}

void Chess::update(dam::Context& ctx)
{
    update_uci();

    update_input(ctx);
    update_promotion(ctx);
    update_ai();
    update_tweening(ctx);
}

void Chess::event(dam::Context& ctx, dam::EventType event)
{
    switch (event) {
    case dam::EventType::WindowResize:
        handle_resize(ctx);
        break;
    default:
        break;
    }
}

void Chess::draw(dam::Context& ctx)
{
    using namespace dam::graphics;

    clear(palette::black);

    draw_board(ctx);
    draw_coordinates(ctx);
    draw_danger_zone(ctx);
    draw_recent_move_indicator(ctx);
    draw_king_safety(ctx);
    draw_selection(ctx);
    draw_moves(ctx);
    draw_pieces(ctx);
    draw_move_animation(ctx);
    draw_promotion(ctx);
}

void Chess::destroy(dam::Context& ctx)
{
    (void)ctx;

    using namespace dam::graphics;

    unload_font(m_font);
    unload_texture(m_pieces);
}
