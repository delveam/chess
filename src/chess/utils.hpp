#include <string>

const int BOARD_WIDTH = 8;
const int BOARD_HEIGHT = 8;

const std::string STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

enum class PieceType {
    None = 0,
    Pawn = 4,
    Bishop = 8,
    Knight = 16,
    Rook = 32,
    Queen = 64,
    King = 128,
};

enum class Team {
    None = 0,
    White = 1,
    Black = 2,
};
