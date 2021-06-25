#ifndef BOARD_HPP
#define BOARD_HPP
#include <array>
#include <optional>
#include <string>
#include "piece.hpp"

// REMARK(thismarvin): I am not sure if this is a good idea, but it certainly saves on typing!
typedef std::array<Piece, constants::board_width * constants::board_height> BoardArray;

class Board {
public:
    Board();
    Board(BoardArray pieces, Team next_team, CastlingRights castling_rights, std::optional<std::string> en_passant_target, unsigned int half_moves, unsigned int full_moves);

    const BoardArray& pieces() const
    {
        return m_pieces;
    }
    const Team& next_team() const
    {
        return m_next_team;
    }
    const CastlingRights& castling_rights() const
    {
        return m_castling_rights;
    }
    const std::optional<std::string>& en_passant_target() const
    {
        return m_en_passant_target;
    }
    const unsigned int& half_moves() const
    {
        return m_half_moves;
    }
    const unsigned int& full_moves() const
    {
        return m_full_moves;
    }

    std::optional<Piece> get(unsigned int x, unsigned int y) const;
    std::optional<Board> move_uci(std::string notation) const;

    static std::optional<Board> load_from_fen(std::string fen);
    static std::optional<std::string> into_fen(Board board);
private:
    BoardArray m_pieces;
    Team m_next_team;
    CastlingRights m_castling_rights;
    std::optional<std::string> m_en_passant_target;
    unsigned int m_half_moves;
    unsigned int m_full_moves;
};

#endif
