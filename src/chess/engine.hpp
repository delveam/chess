#ifndef ENGINE_HPP
#define ENGINE_HPP
#include <future>
#include <queue>
#include <unordered_map>
#include <vector>
#include "board.hpp"
#include "gm.hpp"
#include "move.hpp"

namespace engine {
enum class CommandType {
    None,
    uci,
    ucinewgame,
    isready,
    position,
    go,
    quit
};
enum class Strategy {
    None,
    Maximizing,
    Minimizing
};

class Command;
class Depth;
class Fisher;
class Millisecond;
class Pescado;
class Suggestion;
}

class engine::Command {
public:
    Command() = default;
    Command(CommandType type) :
        m_type(type)
    {
    }
    Command(CommandType type, std::string data) :
        m_type(type),
        m_data(data)
    {
    }

    CommandType type() const
    {
        return m_type;
    }
    std::string data() const
    {
        return m_data;
    }
private:
    CommandType m_type { CommandType::None };
    std::string m_data { "" };
};

class engine::Depth {
public:
    static std::optional<Depth> create(unsigned int depth);

    unsigned int value() const
    {
        return m_depth;
    }
private:
    Depth() = default;
    Depth(unsigned int depth) :
        m_depth(depth)
    {
    }

    unsigned int m_depth { 0 };
};

class engine::Millisecond {
public:
    static std::optional<Millisecond> create(unsigned int milliseconds);
private:
    Millisecond() = default;
    Millisecond(unsigned int milliseconds) :
        m_milliseconds(milliseconds)
    {
    }

    unsigned int m_milliseconds { 0 };
};

class engine::Suggestion {
public:
    Suggestion(Move best_move, Move ponder) :
        m_best_move(best_move),
        m_ponder(ponder)
    {
    }

    Move best_move() const
    {
        return m_best_move;
    }
    Move ponder() const
    {
        return m_ponder;
    }

    std::string to_string() const
    {
        return "bestmove " + m_best_move.lan() + " ponder " + m_ponder.lan();
    }
private:
    Move m_best_move;
    Move m_ponder;
};

class engine::Fisher {
public:
    Fisher() = default;
    Fisher(Board board) :
        m_board(board)
    {
    }

    Board& board()
    {
        return m_board;
    }

    Suggestion go(Depth depth);
    Suggestion go(Millisecond milliseconds);
    Suggestion random();
private:
    Board m_board { Board::create(constants::starting_fen).value() };
    // std::unordered_map<std::string, float> m_cache; // This is basically just for transpositions.

    static int resolve_captures(const Board& board, unsigned int depth, int alpha, int beta, unsigned int& searched);
    static int evaluate(const Board& board);
    static int evaluate_fast(const Board& board);
    static int minimax(const Board& board, unsigned int depth, unsigned int depth2, int alpha, int beta, Strategy strategy, unsigned int& searched);
    static int minimax2(const Board& board, const gm::Analysis& analysis, unsigned int depth, int alpha, int beta, Strategy strategy, unsigned int& searched);
};

class engine::Pescado {
public:
    Pescado() = default;

    void subscribe(std::function<void(std::string)> listener);
    bool submit(std::string command);

    void update();
    void run();

    static int calculate_total_moves(const Board& board, unsigned int depth);
private:
    std::queue<Command> m_commands;

    std::vector<std::function<void(std::string)>> m_listeners;

    std::vector<std::future<std::string>> m_jobs;
    std::queue<int> m_job_deletion;

    bool m_loop { true };
    bool m_ready { false };

    Fisher m_fisher;

    void broadcast(std::string message);
    void resolve_commands();
    void resolve_jobs();
};
#endif
