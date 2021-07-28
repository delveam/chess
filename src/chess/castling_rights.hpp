#ifndef CASTLING_RIGHTS_HPP
#define CASTLING_RIGHTS_HPP
enum class CastlingRights {
    None = 0,
    WhiteKingSide = 1,
    WhiteQueenSide = 2,
    BlackKingSide = 4,
    BlackQueenSide = 8,
};
#endif
