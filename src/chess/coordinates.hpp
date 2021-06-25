#ifndef COORDINATES_HPP
#define COORDINATES_HPP
#include <optional>
#include <string>

class Coordinates {
public:
    const unsigned int& x() const { return m_x; }
    const unsigned int& y() const { return m_y; }

    static std::optional<Coordinates> create(unsigned int x, unsigned int y);
    static std::string to_string(Coordinates coordinates);
    static std::optional<Coordinates> from_string(std::string notation);
private:
    // Coordinates' constructors are private because it is possible to create invalid coordinates.
    // The only way to create a Coordinates object is to use `Coordinates::create`.
    Coordinates(): m_x(0), m_y(0) {}
    Coordinates(unsigned int x, unsigned int y): m_x(x), m_y(y) {}

    unsigned int m_x;
    unsigned int m_y;
};
#endif
