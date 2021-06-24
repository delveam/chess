#ifndef BOARD_HPP
#define BOARD_HPP
#include <array>
#include <optional>
#include <string>
#include "piece.hpp"

typedef std::array<Piece, constants::board_width * constants::board_height> BoardArray;

class Board {
public:
    Board();
    Board(BoardArray pieces, Team next_team, CastlingRights castling_rights, std::optional<std::string> en_passant_target, unsigned int half_moves, unsigned int full_moves);

    BoardArray pieces;
    Team next_team;
    CastlingRights castling_rights;
    std::optional<std::string> en_passant_target;
    unsigned int half_moves;
    unsigned int full_moves;

    std::optional<Piece> get(unsigned int x, unsigned int y);
    std::optional<Board> move_uci(std::string notation);
    static std::optional<Board> load_from_fen(std::string fen);
    static std::optional<std::string> into_fen(Board board);
};

#endif
