#include <string>

const int BOARD_WIDTH = 8;
const int BOARD_HEIGHT = 8;

const std::string STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

enum class PieceType {
    None = 0,
    Pawn = 1,
    Bishop = 2,
    Knight = 4,
    Rook = 8,
    Queen = 16,
    King = 32,
};

enum class Team {
    White = 0,
    Black = 1,
};
