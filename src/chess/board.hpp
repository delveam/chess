#ifndef BOARD_HPP
#define BOARD_HPP
#include <array>
#include <optional>
#include <string>
#include "piece.hpp"

class Board {
public:
    Board();
    std::array<Piece, BOARD_WIDTH * BOARD_HEIGHT> pieces;
    Team next_team;
    CastlingRights castling_rights;
    std::optional<std::string> en_passant_target;
    unsigned int half_moves;
    unsigned int full_moves;

    std::optional<Piece> get(unsigned int x, unsigned int y);
    void move_uci(std::string notation);
    static Board load_from_fen(std::string fen);
    // TODO(thismarvin): static std::string into_fen(Board board);
};

#endif
