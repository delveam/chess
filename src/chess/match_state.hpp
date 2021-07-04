#ifndef MATCH_STATE_HPP
#define MATCH_STATE_HPP
#include "gm.hpp"

class MatchState {
public:
    const Board& board() const
    {
        return m_board;
    }
    const Moves& moves() const
    {
        return m_moves;
    }

    std::optional<MatchState> submit_move(Move move) const;

    static std::optional<MatchState> create(std::string fen);

private:
    MatchState(Board board, Moves moves) :
        m_board(board),
        m_moves(moves)
    {
    }

    Board m_board;
    Moves m_moves;
};
#endif
