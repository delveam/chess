#include "move.hpp"

const Move& Move::nullmove = Move { "0000", Coordinates::create(0, 0).value(), Coordinates::create(0, 0).value(), std::nullopt };

std::optional<Move> Move::create(std::string lan)
{
    if (lan.length() > 5) {
        return std::nullopt;
    }

    auto start = Coordinates::from_string(lan.substr(0, 2));

    if (!start.has_value()) {
        return std::nullopt;
    }

    auto end = Coordinates::from_string(lan.substr(2, 2));

    if (!end.has_value()) {
        return std::nullopt;
    }

    std::optional<PieceType> promotion = std::nullopt;

    if (lan.length() == 5) {
        auto temp = lan.substr(4, 1).c_str()[0];

        // TODO(thismarvin): It might be worth it to put this in utils!
        switch (temp) {
        case 'b': {
            promotion = PieceType::Bishop;
            break;
        }
        case 'n': {
            promotion = PieceType::Knight;
            break;
        }
        case 'r': {
            promotion = PieceType::Rook;
            break;
        }
        case 'q': {
            promotion = PieceType::Queen;
            break;
        }
        default:
            break;
        }
    }

    return Move(lan, start.value(), end.value(), promotion);
}
