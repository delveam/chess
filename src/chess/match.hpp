#ifndef MATCH_HPP
#define MATCH_HPP
#include <vector>
#include "match_state.hpp"
#include "move.hpp"

class Match {
public:
    Match() = default;
    Match(std::vector<std::pair<Move, MatchState>> state) :
        m_state(state)
    {
    }

    const Board& board() const
    {
        return m_state.at(m_head).second.board();
    }
    const Moves& moves() const
    {
        return m_state.at(m_head).second.moves();
    }
    }

    std::optional<bool> submit_move(std::string notation);
    void undo();
    void redo();

    static Match create();
    // TODO(thismarvin): setup `static std::optional<Match> from_pgn(std::string pgn);`
private:
    unsigned int m_head { 0 };
    unsigned int m_end { 0 };
    std::vector<std::pair<Move, MatchState>> m_state;

    // TODO(thismarvin): Implement a PGN system and include it here!
};
#endif
