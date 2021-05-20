#include <regex>
#include "coordinates.hpp"
#include "utils.hpp"

std::optional<Coordinates> Coordinates::create(unsigned int x, unsigned int y)
{
    if (x >= BOARD_WIDTH || y >= BOARD_HEIGHT) {
        return std::nullopt;
    }

    auto result = Coordinates(x, y);

    return result;
}

std::string Coordinates::to_string(Coordinates coordinates)
{
    std::string result = "";
    result.push_back('a' + coordinates.x);
    result.push_back('0' + BOARD_HEIGHT - coordinates.y);

    return result;
}

std::optional<Coordinates> Coordinates::from_string(std::string notation)
{
    if (!std::regex_match(notation, std::regex("[a-h]{1}[1-8]{1}"))) {
        return std::nullopt;
    }

    auto temp = notation.substr(0, 1).c_str()[0];
    temp = tolower(temp);

    auto column = BOARD_WIDTH - ((int)'h' - (int)temp) - 1;
    auto row = BOARD_HEIGHT - std::stoi(notation.substr(1, 2));

    return Coordinates::create(column, row);
}
