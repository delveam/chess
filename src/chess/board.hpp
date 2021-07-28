#ifndef BOARD_HPP
#define BOARD_HPP
#include <array>
#include <optional>
#include <string>
#include "castling_rights.hpp"
#include "constants.hpp"
#include "coordinates.hpp"
#include "piece.hpp"

typedef std::array<Piece, constants::board_size> Pieces;

class Board {
public:
    Board() = default;
    Board(Pieces pieces, Team current_team, CastlingRights castling_rights, std::optional<std::string> en_passant_target, unsigned int half_moves, unsigned int full_moves) :
        m_pieces(pieces),
        m_current_team(current_team),
        m_castling_rights(castling_rights),
        m_en_passant_target(en_passant_target),
        m_half_moves(half_moves),
        m_full_moves(full_moves)
    {
    }

    const Pieces& pieces() const
    {
        return m_pieces;
    }
    Team current_team() const
    {
        return m_current_team;
    }
    CastlingRights castling_rights() const
    {
        return m_castling_rights;
    }
    std::optional<std::string> en_passant_target() const
    {
        return m_en_passant_target;
    }
    unsigned int half_moves() const
    {
        return m_half_moves;
    }
    unsigned int full_moves() const
    {
        return m_full_moves;
    }

    std::optional<Piece> get(unsigned int x, unsigned int y) const;

    static std::optional<Board> from_fen(std::string fen);
    static std::optional<std::string> into_fen(Board board);
private:
    Pieces m_pieces;
    Team m_current_team { Team::None };
    CastlingRights m_castling_rights { CastlingRights::None };
    std::optional<std::string> m_en_passant_target { std::nullopt };
    unsigned int m_half_moves { 0 };
    unsigned int m_full_moves { 1 };
};
#endif
