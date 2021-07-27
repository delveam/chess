#ifndef ENGINE_HPP
#define ENGINE_HPP
#include <queue>
#include <unordered_map>
#include <vector>
#include "board.hpp"
#include "move.hpp"

// uci protocol:
// > uci
// > ucinewgame
// > position startpos
// > go movetime 1000
// > info ...
// > "
// > bestmove d2d4 ponder d7d5
//
// Data Structures Planning:
// - MovePair(Move, Move)
// - UCI (namespace?)
// - Analysis?
//   - MovePair?
//   - Evaluation
// - Worker
//   - Properties:
//     - Options?
//     - Board
//     - Cache : HashMap<"FEN + LAN", Evaluation>????
//   - Methods:
//     - Worker create()
//     - O<Worker> from_position(fen)
//     - O<Worker> from_sequence(fen, moves)
//     - Worker& set_option?
//     - Worker& position(fen, moves?)
//     - MovePair go(depth | move_time)
//     - Analysis eval()
// - Command (std::variant?)
//   - uci
//   - ucinewgame
//   - position
//   - go
//   - eval

namespace engine {
class Depth;
class Millisecond;
class Suggestion;
// class Command;
class Fisher;
class Pescado;
}

class engine::Depth {
public:
    static std::optional<Depth> create(unsigned int depth);
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
private:
    Move m_best_move;
    Move m_ponder;
};

class engine::Fisher {
public:
    Suggestion go(Depth depth);
    Suggestion go(Millisecond millisecond);

    static Fisher create();
    static std::optional<Fisher> from_position(std::string fen);
    static std::optional<Fisher> from_sequence(std::string fen, std::vector<Move> moves);
private:
    Fisher() = default;
    Fisher(Board board) :
        m_board(board)
    {
    }

    Board m_board;
    // std::unordered_map<std::string, float> m_cache;
};

class engine::Pescado {
public:
    Pescado() = default;

    void submit(std::string command);
    void update();
private:
    std::optional<Fisher> m_fisher { std::nullopt };
    std::queue<std::string> m_commands;
};
#endif
