#ifndef COORDINATES_HPP
#define COORDINATES_HPP
#include <optional>
#include <string>

class Coordinates {
public:
    unsigned int x;
    unsigned int y;

    static std::optional<Coordinates> create(unsigned int x, unsigned int y);
    static std::string to_string(Coordinates coordinates);
    static std::optional<Coordinates> from_string(std::string notation);
private:
    Coordinates(): x(0), y(0) {}
    Coordinates(unsigned int x, unsigned int y): x(x), y(y) {}
};
#endif
