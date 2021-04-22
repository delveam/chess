#include "board.hpp"

Board::Board()
{
    pieces.fill({ utils::ChessPiece::None, utils::Player::Black });

    auto i = -1;
    pieces[++i] = { utils::ChessPiece::Rook, utils::Player::Black };
    pieces[++i] = { utils::ChessPiece::Knight, utils::Player::Black };
    pieces[++i] = { utils::ChessPiece::Bishop, utils::Player::Black };
    pieces[++i] = { utils::ChessPiece::Queen, utils::Player::Black };
    pieces[++i] = { utils::ChessPiece::King, utils::Player::Black };
    pieces[++i] = { utils::ChessPiece::Bishop, utils::Player::Black };
    pieces[++i] = { utils::ChessPiece::Knight, utils::Player::Black };
    pieces[++i] = { utils::ChessPiece::Rook, utils::Player::Black };
    pieces[++i] = { utils::ChessPiece::Pawn, utils::Player::Black };
    pieces[++i] = { utils::ChessPiece::Pawn, utils::Player::Black };
    pieces[++i] = { utils::ChessPiece::Pawn, utils::Player::Black };
    pieces[++i] = { utils::ChessPiece::Pawn, utils::Player::Black };
    pieces[++i] = { utils::ChessPiece::Pawn, utils::Player::Black };
    pieces[++i] = { utils::ChessPiece::Pawn, utils::Player::Black };
    pieces[++i] = { utils::ChessPiece::Pawn, utils::Player::Black };
    pieces[++i] = { utils::ChessPiece::Pawn, utils::Player::Black };

    i = 47;
    pieces[++i] = { utils::ChessPiece::Pawn, utils::Player::White };
    pieces[++i] = { utils::ChessPiece::Pawn, utils::Player::White };
    pieces[++i] = { utils::ChessPiece::Pawn, utils::Player::White };
    pieces[++i] = { utils::ChessPiece::Pawn, utils::Player::White };
    pieces[++i] = { utils::ChessPiece::Pawn, utils::Player::White };
    pieces[++i] = { utils::ChessPiece::Pawn, utils::Player::White };
    pieces[++i] = { utils::ChessPiece::Pawn, utils::Player::White };
    pieces[++i] = { utils::ChessPiece::Pawn, utils::Player::White };
    pieces[++i] = { utils::ChessPiece::Rook, utils::Player::White };
    pieces[++i] = { utils::ChessPiece::Knight, utils::Player::White };
    pieces[++i] = { utils::ChessPiece::Bishop, utils::Player::White };
    pieces[++i] = { utils::ChessPiece::Queen, utils::Player::White };
    pieces[++i] = { utils::ChessPiece::King, utils::Player::White };
    pieces[++i] = { utils::ChessPiece::Bishop, utils::Player::White };
    pieces[++i] = { utils::ChessPiece::Knight, utils::Player::White };
    pieces[++i] = { utils::ChessPiece::Rook, utils::Player::White };
}
