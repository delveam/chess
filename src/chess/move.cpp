#include <regex>
#include "move.hpp"

const Move& Move::nullmove = Move { "0000", Coordinates::create(0, 0).value(), Coordinates::create(0, 0).value(), std::nullopt };

std::optional<Move> Move::create(std::string lan)
{
    static const auto regex = std::regex("^([a-h]{1}[1-8]{1}){2}[bnrq]?$");

    if (!std::regex_match(lan, regex)) {
        return std::nullopt;
    }

    auto start = Coordinates::from_string(lan.substr(0, 2)).value();
    auto end = Coordinates::from_string(lan.substr(2, 2)).value();

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

    return Move(lan, start, end, promotion);
}
