#include <regex>
#include "move.hpp"

std::optional<Move> Move::create(std::string lan)
{
    if (!std::regex_match(lan, std::regex("^([a-h]{1}[1-8]{1}){2}[bnrq]?$"))) {
        return std::nullopt;
    }

    auto start = Coordinates::from_string(lan.substr(0, 2));
    auto end = Coordinates::from_string(lan.substr(2, 4));

    std::optional<PieceType> promotion = std::nullopt;

    if (lan.length() == 5) {
        auto temp = lan.substr(4, 5).c_str()[0];

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
