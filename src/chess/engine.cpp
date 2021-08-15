#include <algorithm>
#include <limits>
#include <regex>
#include <thread>
#include "engine.hpp"
#include "utils.hpp"

#include <iostream>

constexpr int checkmate_value = 10000;

int piece_value(Piece piece)
{
    switch (piece.type()) {
    case PieceType::Pawn:
        return 100;
    case PieceType::Bishop:
        return 300;
    case PieceType::Knight:
        return 300;
    case PieceType::Rook:
        return 500;
    case PieceType::Queen:
        return 900;
    case PieceType::King:
        return 0;
    default:
        return 0;
    }
}

int engine::Pescado::calculate_total_moves(const Board& board, unsigned int depth)
{
    if (depth == 0) {
        return 0;
    }

    auto total = 0;

    // auto target_depth = depth;
    auto analysis = gm::analyze(board, board.current_team()).value();

    for (const auto& pair : analysis.moves()) {
        for (const auto& move : pair.second) {
            auto temp_move = gm::apply_move(board, Move::create(move).value());
            auto temp = calculate_total_moves(temp_move, depth - 1);

            if (depth == 1) {
                temp += 1;
            }

            total += temp;

            // TODO(thismarvin): We could give this a permanant home if we implement `go perft`
            // if (depth == target_depth) {
            //     std::cout << move << " : " << temp << "\n";
            // }
        }
    }

    return total;
}

int engine::Fisher::quiesce(const Board& board, int alpha, int beta, Strategy strategy, unsigned int& searched)
{
    auto analysis = gm::analyze(board, board.current_team()).value();

    if (analysis.king_safety() == gm::KingSafety::Checkmate) {
        return board.current_team() == Team::White ? -checkmate_value : checkmate_value;
    }
    if (analysis.king_safety() == gm::KingSafety::Stalemate) {
        return 0;
    }
    if (analysis.king_safety() == gm::KingSafety::Check) {
        return minimax_quiet(board, analysis, alpha, beta, strategy, searched);
    }

    auto stand_pat = evaluate(board);

    if (strategy == Strategy::Maximizing) {
        if (stand_pat >= beta) {
            return beta;
        }

        if (stand_pat > alpha) {
            alpha = stand_pat;
        }
    }
    if (strategy == Strategy::Minimizing) {
        if (stand_pat <= alpha) {
            return alpha;
        }

        if (stand_pat < beta) {
            beta = stand_pat;
        }
    }

    auto opponent = board.current_team() == Team::White ? Team::Black : Team::White;

    std::vector<std::pair<int, Board>> states;

    for (const auto& pair : analysis.moves()) {
        for (const auto& move : pair.second) {
            auto temp_move = Move::create(move).value();
            auto temp_board = gm::apply_move(board, temp_move);
            auto temp_end = board.pieces().at(temp_move.end().y() * constants::board_width + temp_move.end().x());

            if (temp_end.team() == opponent) {
                auto temp_start = board.pieces().at(temp_move.start().y() * constants::board_width + temp_move.start().x());
                auto score = 0;

                if (temp_start.type() == PieceType::King) {
                    score += 800;
                }
                else {
                    score += 900 + piece_value(temp_end) - piece_value(temp_start);
                }

                states.push_back(std::make_pair(score, temp_board));
            }
        }
    }

    if ((int)states.size() == 0) {
        if (strategy == Strategy::Maximizing) {
            return alpha;
        }
        if (strategy == Strategy::Minimizing) {
            return beta;
        }

        return 0;
    }

    std::sort(states.begin(), states.end(), [&](std::pair<int, Board> a, std::pair<int, Board> b) {
        return a.first > b.first;
    });

    auto next_strategy = strategy == Strategy::Maximizing ? Strategy::Minimizing : Strategy::Maximizing;

    for (int i = 0; i < (int)states.size(); ++i) {
        searched += 1;
        auto score = quiesce(states.at(i).second, alpha, beta, next_strategy, searched);

        if (strategy == Strategy::Maximizing) {
            if (score >= beta) {
                return beta;
            }

            if (score > alpha) {
                alpha = score;
            }
        }
        if (strategy == Strategy::Minimizing) {
            if (score <= alpha) {
                return alpha;
            }

            if (score < beta) {
                beta = score;
            }
        }
    }

    if (strategy == Strategy::Maximizing) {
        return alpha;
    }
    if (strategy == Strategy::Minimizing) {
        return beta;
    }

    return 0;
}

int engine::Fisher::evaluate(const Board& board)
{
    auto white_analysis = gm::analyze(board, Team::White);
    auto black_analysis = gm::analyze(board, Team::Black);

    if (white_analysis->king_safety() == gm::KingSafety::Stalemate || black_analysis->king_safety() == gm::KingSafety::Stalemate) {
        return 0;
    }

    if (white_analysis->king_safety() == gm::KingSafety::Checkmate) {
        return -checkmate_value;
    }
    if (black_analysis->king_safety() == gm::KingSafety::Checkmate) {
        return checkmate_value;
    }

    auto white_score = 0.0;
    auto black_score = 0.0;

    white_score += black_analysis->king_safety() == gm::KingSafety::Check ? 75 : 0;
    black_score += white_analysis->king_safety() == gm::KingSafety::Check ? 75 : 0;

    for (int y = 0; y < constants::board_height; ++y) {
        for (int x = 0; x < constants::board_width; ++x) {
            auto target = board.get(x, y);

            if (target.has_value()) {
                if (target->team() == Team::White) {
                    white_score += piece_value(target.value());
                }
                else {
                    black_score += piece_value(target.value());
                }
            }
        }
    }

    auto white_starting_rank = 0;
    auto black_starting_rank = 0;

    for (int x = 0; x < constants::board_width; ++x) {
        auto target_white = board.get(x, constants::board_height - 1).value();
        auto target_black = board.get(x, 0).value();

        if (target_white.team() == Team::White) {
            white_starting_rank += 1;
        }
        if (target_black.team() == Team::Black) {
            black_starting_rank += 1;
        }
    }

    white_score -= white_starting_rank * 10;
    black_score -= black_starting_rank * 10;

    auto white_total_moves = 0;
    auto black_total_moves = 0;

    for (const auto& pair : white_analysis->moves()) {
        white_total_moves += pair.second.size();
    }

    for (const auto& pair : black_analysis->moves()) {
        black_total_moves += pair.second.size();
    }

    white_score += (int)(white_total_moves * 0.25) - (int)(black_total_moves * 0.125);
    black_score += (int)(black_total_moves * 0.25) - (int)(white_total_moves * 0.125);

    return white_score - black_score;
}

int engine::Fisher::evaluate_fast(const Board& board)
{
    auto white_score = 0.0;
    auto black_score = 0.0;

    for (int y = 0; y < constants::board_height; ++y) {
        for (int x = 0; x < constants::board_width; ++x) {
            auto target = board.get(x, y);

            if (target.has_value()) {
                if (target->team() == Team::White) {
                    white_score += piece_value(target.value());
                }
                else {
                    black_score += piece_value(target.value());
                }
            }
        }
    }

    return white_score - black_score;
}

int engine::Fisher::minimax(const Board& board, unsigned int depth, int alpha, int beta, Strategy strategy, unsigned int& searched)
{
    if (depth == 0) {
        return engine::Fisher::quiesce(board, alpha, beta, strategy, searched);
    }

    auto analysis = gm::analyze(board, board.current_team()).value();

    if (analysis.king_safety() == gm::KingSafety::Checkmate) {
        return board.current_team() == Team::White ? -checkmate_value : checkmate_value;
    }
    if (analysis.king_safety() == gm::KingSafety::Stalemate) {
        return 0;
    }

    auto opponent = board.current_team() == Team::White ? Team::Black : Team::White;
    auto needs_sorting = false;

    std::vector<std::pair<int, Board>> states;

    for (const auto& pair : analysis.moves()) {
        for (const auto& move : pair.second) {
            auto temp_move = Move::create(move).value();
            auto temp_board = gm::apply_move(board, temp_move);
            auto temp_end = board.pieces().at(temp_move.end().y() * constants::board_width + temp_move.end().x());

            auto score = 0;

            if (temp_end.team() == opponent) {
                auto temp_start = board.pieces().at(temp_move.start().y() * constants::board_width + temp_move.start().x());

                if (temp_start.type() == PieceType::King) {
                    score += 800;
                }
                else {
                    score += 900 + piece_value(temp_end) - piece_value(temp_start);
                }

                needs_sorting = true;
            }

            states.push_back(std::make_pair(score, temp_board));
        }
    }

    if (needs_sorting) {
        std::sort(states.begin(), states.end(), [&](std::pair<int, Board> a, std::pair<int, Board> b) {
            return a.first > b.first;
        });
    }

    auto next_strategy = strategy == Strategy::Maximizing ? Strategy::Minimizing : Strategy::Maximizing;

    for (int i = 0; i < (int)states.size(); ++i) {
        searched += 1;

        auto score = minimax(states.at(i).second, depth - 1, alpha, beta, next_strategy, searched);

        if (strategy == Strategy::Maximizing) {
            if (score >= beta) {
                return beta;
            }
            if (score > alpha) {
                alpha = score;
            }
        }
        if (strategy == Strategy::Minimizing) {
            if (score <= alpha) {
                return alpha;
            }
            if (score < beta) {
                beta = score;
            }
        }
    }

    if (strategy == Strategy::Maximizing) {
        return alpha;
    }
    if (strategy == Strategy::Minimizing) {
        return beta;
    }

    return 0;
}

int engine::Fisher::minimax_quiet(const Board& board, const gm::Analysis& analysis, int alpha, int beta, Strategy strategy, unsigned int& searched)
{
    auto opponent = board.current_team() == Team::White ? Team::Black : Team::White;
    auto needs_sorting = false;

    std::vector<std::pair<int, Board>> states;

    for (const auto& pair : analysis.moves()) {
        for (const auto& move : pair.second) {
            auto temp_move = Move::create(move).value();
            auto temp_board = gm::apply_move(board, temp_move);
            auto temp_end = board.pieces().at(temp_move.end().y() * constants::board_width + temp_move.end().x());

            auto score = 0;

            if (temp_end.team() == opponent) {
                auto temp_start = board.pieces().at(temp_move.start().y() * constants::board_width + temp_move.start().x());

                if (temp_start.type() == PieceType::King) {
                    score += 800;
                }
                else {
                    score += 900 + piece_value(temp_end) - piece_value(temp_start);
                }

                needs_sorting = true;
            }

            states.push_back(std::make_pair(score, temp_board));
        }
    }

    if (needs_sorting) {
        std::sort(states.begin(), states.end(), [&](std::pair<int, Board> a, std::pair<int, Board> b) {
            return a.first > b.first;
        });
    }

    auto next_strategy = strategy == Strategy::Maximizing ? Strategy::Minimizing : Strategy::Maximizing;

    for (int i = 0; i < (int)states.size(); ++i) {
        searched += 1;

        auto score = quiesce(states.at(i).second, alpha, beta, next_strategy, searched);

        if (strategy == Strategy::Maximizing) {
            if (score >= beta) {
                return beta;
            }
            if (score > alpha) {
                alpha = score;
            }
        }
        if (strategy == Strategy::Minimizing) {
            if (score <= alpha) {
                return alpha;
            }
            if (score < beta) {
                beta = score;
            }
        }
    }

    if (strategy == Strategy::Maximizing) {
        return alpha;
    }
    if (strategy == Strategy::Minimizing) {
        return beta;
    }

    return 0;
}

std::optional<engine::Depth> engine::Depth::create(unsigned int depth)
{
    // This constraint is arbitrary.
    if (depth > 64) {
        return std::nullopt;
    }

    return engine::Depth(depth);
}

std::optional<engine::Millisecond> engine::Millisecond::create(unsigned int milliseconds)
{
    return engine::Millisecond(milliseconds);
}

engine::Suggestion engine::Fisher::go(Depth depth)
{
    auto strategy = m_board.current_team() == Team::White ? Strategy::Minimizing : Strategy::Maximizing;
    auto best_move = Move::nullmove;

    unsigned int nodes_searched = 0;

    auto analysis = gm::analyze(m_board, m_board.current_team()).value();

    auto alpha = std::numeric_limits<int>::lowest();
    auto beta = std::numeric_limits<int>::max();

    auto best_score = m_board.current_team() == Team::White ? alpha : beta;

    for (const auto& pair : analysis.moves()) {
        for (const auto& move : pair.second) {
            auto temp_move = Move::create(move).value();

            nodes_searched += 1;
            auto score = minimax(gm::apply_move(m_board, temp_move), depth.value() - 1, alpha, beta, strategy, nodes_searched);

            if (m_board.current_team() == Team::White) {
                if (score > best_score) {
                    best_score = score;
                    best_move = temp_move;
                }

                if (score > alpha) {
                    alpha = score;
                }

                if (beta <= alpha) {
                    break;
                }
            }
            if (m_board.current_team() == Team::Black) {
                if (score < best_score) {
                    best_score = score;
                    best_move = temp_move;
                }

                if (score < beta) {
                    beta = score;
                }

                if (beta <= alpha) {
                    break;
                }
            }
        }
    }

    // TODO(thismarvin): How can we broadcast this to the engine?
    std::cout << "info depth " << depth.value() << " nodes " << nodes_searched << " score cp " << best_score << "\n";
    // auto message = "info depth " + std::to_string(depth.value()) + " nodes " + std::to_string(nodes_searched) + " score cp " + std::to_string(best_score);

    return Suggestion(best_move, Move::nullmove);
}

engine::Suggestion engine::Fisher::go(engine::Millisecond milliseconds)
{
    // TODO(thismarvin): Everthing!

    return Suggestion(Move::nullmove, Move::nullmove);
}

engine::Suggestion engine::Fisher::random()
{
    auto random_range = [](int min, int max) {
        return min + rand() % (max - min);
    };

    auto analysis = gm::analyze(m_board, m_board.current_team()).value();

    std::vector<unsigned int> potential_pieces;

    for (auto const& pair : analysis.moves()) {
        potential_pieces.push_back(pair.first);
    }

    std::optional<unsigned int> piece_index = std::nullopt;
    std::optional<std::string> move = std::nullopt;

    while (!move.has_value()) {
        piece_index = potential_pieces[random_range(0, (int)potential_pieces.size())];
        MoveSet move_set = analysis.moves().at(piece_index.value());

        if (move_set.size() == 0) {
            continue;
        }

        int temp = random_range(0, (int)move_set.size());

        int i = 0;
        for (const auto& entry : move_set) {
            if (i == temp) {
                move = entry;
                break;
            }

            i += 1;
        }
    }

    return Suggestion(Move::create(move.value()).value(), Move::nullmove);
}

bool engine::Pescado::submit(std::string command)
{
    static const auto regex_uci = std::regex("^uci$");
    static const auto regex_ucinewgame = std::regex("^ucinewgame$");
    static const auto regex_isready = std::regex("^isready$");
    static const auto regex_position = std::regex("^position\\s+(startpos|fen\\s+" + constants::fen_regex + ")(\\s+moves(\\s+([a-h][1-8]){2})+)?$");
    static const auto regex_go = std::regex("^go\\s+(depth|movetime)\\s+\\d+$");
    static const auto regex_quit = std::regex("^quit$");

    if (std::regex_match(command, regex_uci)) {
        m_commands.push(engine::Command(CommandType::uci, ""));

        return true;
    }

    if (!m_ready) {
        return false;
    }

    if (std::regex_match(command, regex_ucinewgame)) {
        m_commands.push(engine::Command(CommandType::ucinewgame, ""));

        return true;
    }
    if (std::regex_match(command, regex_isready)) {
        m_commands.push(engine::Command(CommandType::isready, ""));

        return true;
    }
    if (std::regex_match(command, regex_position)) {
        auto args = utils::split_whitespace(command);

        std::string sanitized = args.at(1);

        for (int i = 2; i < (int)args.size(); ++i) {
            sanitized += " " + args.at(i);
        }

        m_commands.push(engine::Command(CommandType::position, sanitized));

        return true;
    }
    if (std::regex_match(command, regex_go)) {
        auto args = utils::split_whitespace(command);

        std::string sanitized = args.at(1);

        for (int i = 2; i < (int)args.size(); ++i) {
            sanitized += " " + args.at(i);
        }

        m_commands.push(engine::Command(CommandType::go, sanitized));

        return true;
    }
    if (std::regex_match(command, regex_quit)) {
        m_commands.push(engine::Command(CommandType::quit, ""));

        return true;
    }

    return false;
}

void engine::Pescado::broadcast(std::string message)
{
    for (int i = 0; i < (int)m_listeners.size(); ++i) {
        m_listeners.at(i)(message);
    }
}

void engine::Pescado::resolve_commands()
{
    while (m_commands.size() > 0) {
        auto command = m_commands.front();

        switch (command.type()) {
        case engine::CommandType::uci: {
            broadcast("id name Pescado");
            broadcast("id author the Pescado developers");
            broadcast("uciok");

            m_ready = true;

            auto board = Board::create(constants::starting_fen).value();

            break;
        }
        case engine::CommandType::isready: {
            broadcast("readyok");

            break;
        }
        case engine::CommandType::ucinewgame: {
            m_fisher = Fisher();

            break;
        }
        case engine::CommandType::position: {
            auto temp = utils::split_whitespace(command.data());

            std::queue<std::string> args;

            for (int i = 0; i < (int)temp.size(); ++i) {
                args.push(temp.at(i));
            }

            auto fen = constants::starting_fen;
            auto board = Board::create(fen);

            if (args.front() == "startpos") {
                args.pop();
            }
            else if (args.front() == "fen") {
                args.pop();

                fen = args.front();
                args.pop();

                for (int i = 1; i < 6; ++i) {
                    fen += " " + args.front();
                    args.pop();
                }

                board = Board::create(fen);
            }

            if (args.size() > 0) {
                args.pop();

                std::string moves = args.front();
                args.pop();

                while (args.size() > 0) {
                    moves += " " + args.front();
                    args.pop();
                }

                board = gm::board_from_sequence(fen, moves);
            }

            if (!board.has_value()) {
                break;
            }

            m_fisher.board() = board.value();

            break;
        }
        case engine::CommandType::go: {
            auto args = utils::split_whitespace(command.data());

            // TODO(thismarvin): We need to either prevent multiple jobs or implement locking... right?
            m_jobs.push_back(std::async(std::launch::async, [&, args]() {
                auto result = m_fisher.go(Depth::create(std::stoi(args.at(1))).value());

                return result.to_string();
            }));

            break;
        }
        case engine::CommandType::quit: {
            m_loop = false;
            break;
        }
        default:
            break;
        }

        m_commands.pop();
    }
}

void engine::Pescado::subscribe(std::function<void (std::string)> listener)
{
    m_listeners.push_back(listener);
}

void engine::Pescado::resolve_jobs()
{
    for (int i = 0; i < (int)m_jobs.size(); ++i) {
        if (m_jobs.at(i).valid() && m_jobs.at(i).wait_for(std::chrono::microseconds(0)) == std::future_status::ready) {
            auto result = m_jobs.at(i).get();

            broadcast(result);

            m_job_deletion.push(i);
        }
    }

    while (m_job_deletion.size() > 0) {
        m_jobs.erase(m_jobs.begin() + m_job_deletion.front());
        m_job_deletion.pop();
    }
}

void engine::Pescado::update()
{
    if (!m_loop) {
        return;
    }

    resolve_commands();
    resolve_jobs();
}

void engine::Pescado::run()
{
    while (m_loop) {
        update();
    }
}
