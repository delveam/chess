#ifndef MATCH_STATE_HPP
#define MATCH_STATE_HPP
#include "gm.hpp"

class MatchState {
public:
    const Board& board() const
    {
        return m_board;
    }
    const gm::Analysis& analysis() const
    {
        return m_analysis;
    }

    std::optional<MatchState> submit_move(Move move) const;

    static std::optional<MatchState> create(std::string fen);

private:
    MatchState(Board board, gm::Analysis analysis) :
        m_board(board),
        m_analysis(analysis)
    {
    }

    Board m_board;
    gm::Analysis m_analysis;
};
#endif
