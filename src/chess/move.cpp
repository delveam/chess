#include <regex>
#include "move.hpp"

const Move& Move::nullmove = Move { "0000", Coordinates::create(0, 0).value(), Coordinates::create(0, 0).value(), std::nullopt };

std::optional<Move> Move::create(std::string lan)
{
    if (!std::regex_match(lan, std::regex("^([a-h]{1}[1-8]{1}){2}[bnrq]?$"))) {
        return std::nullopt;
    }

    auto start = Coordinates::from_string(lan.substr(0, 2));
    auto end = Coordinates::from_string(lan.substr(2, 2));

    std::optional<PieceType> promotion = std::nullopt;

    if (lan.length() == 5) {
        auto temp = lan.substr(4, 1).c_str()[0];

        // TODO(thismarvin): It might be worth it to put this in utils!
        switch (temp) {
        case 'b':
            promotion = PieceType::Bishop;
            break;
        case 'n':
            promotion = PieceType::Knight;
            break;
        case 'r':
            promotion = PieceType::Rook;
            break;
        case 'q':
            promotion = PieceType::Queen;
            break;
        default:
            break;
        }
    }

    return Move(lan, start.value(), end.value(), promotion);
}
