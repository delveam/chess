#include "constants.hpp"
#include "coordinates.hpp"

std::string Coordinates::to_string() const
{
    std::string result = "";
    result.push_back('a' + m_x);
    result.push_back('0' + constants::board_height - m_y);

    return result;
}

std::optional<Coordinates> Coordinates::create(unsigned int x, unsigned int y)
{
    if (x >= constants::board_width || y >= constants::board_height) {
        return std::nullopt;
    }

    return Coordinates(x, y);
}

std::optional<Coordinates> Coordinates::from_string(std::string notation)
{
    if (notation.length() != 2) {
        return std::nullopt;
    }

    auto temp = notation.substr(0, 1).c_str()[0];
    temp = tolower(temp);

    auto column = constants::board_width - ((int)'h' - (int)temp) - 1;
    auto row = constants::board_height - std::stoi(notation.substr(1, 1));

    return Coordinates::create(column, row);
}
