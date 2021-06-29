#ifndef BOARD_HPP
#define BOARD_HPP
#include <array>
#include <map>
#include <optional>
#include <set>
#include <string>
#include "coordinates.hpp"
#include "piece.hpp"

// TODO(thismarvin): I feel like these need better names!
typedef std::array<Piece, constants::board_width * constants::board_height> Pieces;
typedef std::set<std::string> MoveSet;
typedef std::map<unsigned int, MoveSet> Moves;

class Board {
public:
    Board();
    Board(Pieces pieces, Team next_team, CastlingRights castling_rights, std::optional<std::string> en_passant_target, unsigned int half_moves, unsigned int full_moves);

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
    const Moves& moves() const
    {
        return m_moves;
    }

    std::optional<Piece> get(unsigned int x, unsigned int y) const;
    std::optional<Board> move_uci(std::string notation) const;

    static std::optional<Board> load_from_fen(std::string fen);
    static std::optional<std::string> into_fen(Board board);
private:
    Pieces m_pieces;
    Team m_current_team { Team::None };
    CastlingRights m_castling_rights { CastlingRights::None };
    std::optional<std::string> m_en_passant_target { std::nullopt };
    unsigned int m_half_moves { 0 };
    unsigned int m_full_moves { 1 };

    // TODO(thismarvin): I feel like the following properties/methods should be decoupled from Board...
    Moves m_moves;
    Moves generate_move_map() const;
    MoveSet generate_pawn_moves(Coordinates coords) const;
    MoveSet generate_knight_moves(Coordinates coords) const;
    MoveSet generate_bishop_moves(Coordinates coords) const;
    MoveSet generate_rook_moves(Coordinates coords) const;
    MoveSet generate_queen_moves(Coordinates coords) const;
    MoveSet generate_king_moves(Coordinates coords) const;
};
#endif
